#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>

enum FaultMode : uint8_t;
enum ImuSelect : uint8_t;
// -----------------------------------------------

/*
  =====================================================
  Dual IMU (BMI088 + ICM-42605) + Auto Failover + Fault Injection (Serial "F")
  Target: Teensy 4.x

  SERIAL OUT (CSV 10 columnas):
    rollP,pitchP,bmi_roll,bmi_pitch,icm_roll,icm_pitch,dRoll,dPitch,mismatch,primary
    primary: 0=BMI, 1=ICM

  SERIAL IN (comandos, terminados en '\n'):
    F0             -> sin fallas
    F1             -> BMI dead (bmi_data_ok=0)
    F2             -> ICM dead (icm_data_ok=0)
    F3             -> BMI corrupt (mismatch)
    F4             -> ICM corrupt
    F5             -> BMI stuck 
    F6             -> ICM stuck (ICM freezing)
    F3,5000        -> apply F3 for 5000 ms, return to F0
  =====================================================
*/

// =====================================================
// IMU selection
// =====================================================
enum ImuSelect : uint8_t { IMU_PRIMARY_BMI = 0, IMU_PRIMARY_ICM = 1 };
static ImuSelect PRIMARY_IMU = IMU_PRIMARY_BMI;

// =====================================================
// Fault injection
// =====================================================
enum FaultMode : uint8_t {
  FAULT_NONE = 0,
  FAULT_BMI_DEAD = 1,
  FAULT_ICM_DEAD = 2,
  FAULT_BMI_CORRUPT = 3,
  FAULT_ICM_CORRUPT = 4,
  FAULT_BMI_STUCK = 5,
  FAULT_ICM_STUCK = 6
};

static FaultMode faultMode = FAULT_NONE;
static uint32_t fault_until_ms = 0; 

static void set_fault(FaultMode m, uint32_t duration_ms) {
  faultMode = m;
  fault_until_ms = (duration_ms > 0) ? (millis() + duration_ms) : 0;
}

static void fault_tick_auto_clear() {
  if (fault_until_ms != 0 && (int32_t)(millis() - fault_until_ms) >= 0) {
    faultMode = FAULT_NONE;
    fault_until_ms = 0;
  }
}

// =====================================================
// I2C addresses
// =====================================================
static const uint8_t BMI_ACC_ADDR = 0x18;
static const uint8_t BMI_GYR_ADDR = 0x68;
static const uint8_t ICM_ADDR     = 0x69;

// I2C clocks
static const uint32_t I2C0_FREQ = 400000; // Wire  (BMI)
static const uint32_t I2C1_FREQ = 100000; // Wire1 (ICM robust)

// =====================================================
// State flags
// =====================================================
bool bmi_ok=false, icm_ok=false;
bool bmi_data_ok=false, icm_data_ok=false;

// Units per IMU
float bmi_ax=0,bmi_ay=0,bmi_az=0, bmi_gx=0,bmi_gy=0,bmi_gz=0; // g, deg/s
float icm_ax=0,icm_ay=0,icm_az=0, icm_gx=0,icm_gy=0,icm_gz=0; // g, deg/s

// Helpers
static inline int16_t le16(uint8_t lsb, uint8_t msb) { return (int16_t)((msb << 8) | lsb); }
static inline int16_t be16(uint8_t msb, uint8_t lsb) { return (int16_t)((msb << 8) | lsb); }
static inline float   rad2deg(float r){ return r * 57.2957795f; }

// =====================================================
// BMI088 regs
// =====================================================
static const uint8_t BMI_ACC_DATA_X_L  = 0x12;
static const uint8_t BMI_ACC_RANGE     = 0x41;
static const uint8_t BMI_ACC_PWR_CONF  = 0x7C;
static const uint8_t BMI_ACC_PWR_CTRL  = 0x7D;
static const uint8_t BMI_ACC_SOFTRESET = 0x7E;

static const uint8_t BMI_GYR_RATE_X_L  = 0x02;
static const uint8_t BMI_GYR_RANGE     = 0x0F;
static const uint8_t BMI_GYR_BANDWIDTH = 0x10;
static const uint8_t BMI_GYR_LPM1      = 0x11;
static const uint8_t BMI_GYR_SOFTRESET = 0x14;

// =====================================================
// ICM-42605 regs (Bank0)
// =====================================================
static const uint8_t ICM_BANK_SEL       = 0x76;
static const uint8_t ICM_DEVICE_CONFIG  = 0x11;
static const uint8_t ICM_WHO_AM_I       = 0x75; // expected 0x42
static const uint8_t ICM_INTF_CONFIG1   = 0x4D;
static const uint8_t ICM_PWR_MGMT0      = 0x4E;
static const uint8_t ICM_GYRO_CONFIG0   = 0x4F;
static const uint8_t ICM_ACCEL_CONFIG0  = 0x50;
static const uint8_t ICM_ACCEL_DATA_X1  = 0x1F;
static const uint8_t ICM_GYRO_DATA_X1   = 0x25;

// =====================================================
// Wire (BMI) helpers
// =====================================================
static bool w_write8(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg); Wire.write(val);
  return (Wire.endTransmission(true) == 0);
}

static bool w_readN(uint8_t addr, uint8_t startReg, uint8_t *buf, size_t n) {
  Wire.beginTransmission(addr);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) return false;
  size_t got = Wire.requestFrom((int)addr, (int)n);
  if (got != n) return false;
  for (size_t i=0;i<n;i++) buf[i]=Wire.read();
  return true;
}

// =====================================================
// Wire1 (ICM) robust helpers
// =====================================================
static bool w1_write8_stop(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire1.beginTransmission(addr);
  Wire1.write(reg); Wire1.write(val);
  return (Wire1.endTransmission(true) == 0);
}

static bool w1_read8_stop(uint8_t addr, uint8_t reg, uint8_t &val) {
  Wire1.beginTransmission(addr);
  Wire1.write(reg);
  if (Wire1.endTransmission(true) != 0) return false;
  delayMicroseconds(80);
  if (Wire1.requestFrom((int)addr, 1) != 1) return false;
  val = Wire1.read();
  return true;
}

static bool w1_read6_bytewise(uint8_t addr, uint8_t startReg, uint8_t out6[6]) {
  for (int i=0;i<6;i++) {
    if (!w1_read8_stop(addr, startReg + i, out6[i])) return false;
  }
  return true;
}

// =====================================================
// BMI init + read
// =====================================================
static bool bmi_init() {
  if (!w_write8(BMI_ACC_ADDR, BMI_ACC_SOFTRESET, 0xB6)) return false;
  delay(2);
  if (!w_write8(BMI_ACC_ADDR, BMI_ACC_PWR_CONF, 0x00)) return false;
  delay(1);
  if (!w_write8(BMI_ACC_ADDR, BMI_ACC_PWR_CTRL, 0x04)) return false;
  delay(5);
  if (!w_write8(BMI_ACC_ADDR, BMI_ACC_RANGE, 0x01)) return false;

  if (!w_write8(BMI_GYR_ADDR, BMI_GYR_SOFTRESET, 0xB6)) return false;
  delay(35);
  if (!w_write8(BMI_GYR_ADDR, BMI_GYR_LPM1, 0x00)) return false;
  delay(1);
  if (!w_write8(BMI_GYR_ADDR, BMI_GYR_RANGE, 0x00)) return false;
  if (!w_write8(BMI_GYR_ADDR, BMI_GYR_BANDWIDTH, 0x02)) return false;
  return true;
}

static bool bmi_read_units(float &ax_g, float &ay_g, float &az_g,
                           float &gx_dps, float &gy_dps, float &gz_dps) {
  uint8_t a[6], g[6];
  if (!w_readN(BMI_ACC_ADDR, BMI_ACC_DATA_X_L, a, 6)) return false;
  if (!w_readN(BMI_GYR_ADDR, BMI_GYR_RATE_X_L, g, 6)) return false;

  int16_t ax = le16(a[0],a[1]);
  int16_t ay = le16(a[2],a[3]);
  int16_t az = le16(a[4],a[5]);

  int16_t gx = le16(g[0],g[1]);
  int16_t gy = le16(g[2],g[3]);
  int16_t gz = le16(g[4],g[5]);

  const float ACC_LSB_PER_G   = 5460.0f;
  const float GYR_LSB_PER_DPS = 16.4f;

  ax_g = (float)ax / ACC_LSB_PER_G;
  ay_g = (float)ay / ACC_LSB_PER_G;
  az_g = (float)az / ACC_LSB_PER_G;

  gx_dps = (float)gx / GYR_LSB_PER_DPS;
  gy_dps = (float)gy / GYR_LSB_PER_DPS;
  gz_dps = (float)gz / GYR_LSB_PER_DPS;
  return true;
}

// =====================================================
// ICM init + read
// =====================================================
static bool icm_init_strict() {
  if (!w1_write8_stop(ICM_ADDR, ICM_BANK_SEL, 0x00)) return false;
  delay(1);

  if (!w1_write8_stop(ICM_ADDR, ICM_DEVICE_CONFIG, 0x01)) return false;
  delay(3);

  if (!w1_write8_stop(ICM_ADDR, ICM_BANK_SEL, 0x00)) return false;
  delay(1);

  uint8_t who=0;
  if (!w1_read8_stop(ICM_ADDR, ICM_WHO_AM_I, who)) return false;
  if (who != 0x42) return false;

  (void)w1_write8_stop(ICM_ADDR, ICM_INTF_CONFIG1, 0x01);
  delay(1);

  if (!w1_write8_stop(ICM_ADDR, ICM_GYRO_CONFIG0,  0x06)) return false;
  if (!w1_write8_stop(ICM_ADDR, ICM_ACCEL_CONFIG0, 0x06)) return false;

  if (!w1_write8_stop(ICM_ADDR, ICM_PWR_MGMT0, 0x0F)) return false;
  delay(70);
  return true;
}

static bool icm_read_units(float &ax_g, float &ay_g, float &az_g,
                           float &gx_dps, float &gy_dps, float &gz_dps) {
  (void)w1_write8_stop(ICM_ADDR, ICM_BANK_SEL, 0x00);
  delayMicroseconds(80);

  uint8_t pm=0;
  if (w1_read8_stop(ICM_ADDR, ICM_PWR_MGMT0, pm)) {
    if (pm != 0x0F) {
      (void)w1_write8_stop(ICM_ADDR, ICM_PWR_MGMT0, 0x0F);
      delay(70);
    }
  }

  uint8_t a[6], g[6];
  if (!w1_read6_bytewise(ICM_ADDR, ICM_ACCEL_DATA_X1, a)) return false;
  if (!w1_read6_bytewise(ICM_ADDR, ICM_GYRO_DATA_X1,  g)) return false;

  int16_t ax = be16(a[0],a[1]);
  int16_t ay = be16(a[2],a[3]);
  int16_t az = be16(a[4],a[5]);

  int16_t gx = be16(g[0],g[1]);
  int16_t gy = be16(g[2],g[3]);
  int16_t gz = be16(g[4],g[5]);

  const float ACC_LSB_PER_G   = 2048.0f;
  const float GYR_LSB_PER_DPS = 16.4f;

  ax_g = (float)ax / ACC_LSB_PER_G;
  ay_g = (float)ay / ACC_LSB_PER_G;
  az_g = (float)az / ACC_LSB_PER_G;

  gx_dps = (float)gx / GYR_LSB_PER_DPS;
  gy_dps = (float)gy / GYR_LSB_PER_DPS;
  gz_dps = (float)gz / GYR_LSB_PER_DPS;
  return true;
}

// =====================================================
// Init + update both
// =====================================================
static void imu_init() {
  Wire.begin();
  Wire.setClock(I2C0_FREQ);

  Wire1.setSDA(17);
  Wire1.setSCL(16);
  Wire1.begin();
  Wire1.setClock(I2C1_FREQ);

  bmi_ok = bmi_init();
  icm_ok = icm_init_strict();
}

static void imu_update() {
  bmi_data_ok = bmi_read_units(bmi_ax,bmi_ay,bmi_az, bmi_gx,bmi_gy,bmi_gz);
  icm_data_ok = icm_read_units(icm_ax,icm_ay,icm_az, icm_gx,icm_gy,icm_gz);
}

// =====================================================
// Complementary angles
// =====================================================
static float bmi_roll=0, bmi_pitch=0;
static float icm_roll=0, icm_pitch=0;
static uint32_t t_prev_us=0;
static const float ALPHA = 0.98f;

// =====================================================
// Mismatch + failover (motion gating)
// =====================================================
static const float MISMATCH_DEG   = 10.0f;
static const int   MISMATCH_HITS  = 15;
static const uint32_t FAILOVER_COOLDOWN_MS = 2000;

static int mismatch_counter = 0;
static bool imu_mismatch = false;
static uint32_t last_failover_ms = 0;

static const float GATE_GYRO_DPS = 60.0f;
static const float GATE_ACC_G    = 0.20f;

static bool is_dynamic_motion() {
  if (!(bmi_data_ok && icm_data_ok)) return false;

  float gB = sqrtf(bmi_gx*bmi_gx + bmi_gy*bmi_gy + bmi_gz*bmi_gz);
  float gI = sqrtf(icm_gx*icm_gx + icm_gy*icm_gy + icm_gz*icm_gz);
  float gMin = min(gB, gI);

  float aB = sqrtf(bmi_ax*bmi_ax + bmi_ay*bmi_ay + bmi_az*bmi_az);
  float aI = sqrtf(icm_ax*icm_ax + icm_ay*icm_ay + icm_az*icm_az);
  float aAvg = 0.5f * (aB + aI);

  return (gMin > GATE_GYRO_DPS) || (fabsf(aAvg - 1.0f) > GATE_ACC_G);
}

static void mismatch_update(float dRoll, float dPitch) {
  if (bmi_data_ok && icm_data_ok) {
    if (fabsf(dRoll) > MISMATCH_DEG || fabsf(dPitch) > MISMATCH_DEG) mismatch_counter++;
    else mismatch_counter--;
  } else {
    mismatch_counter++;
  }
  if (mismatch_counter < 0) mismatch_counter = 0;
  if (mismatch_counter > MISMATCH_HITS) mismatch_counter = MISMATCH_HITS;
  imu_mismatch = (mismatch_counter >= MISMATCH_HITS);
}

static bool primary_dead() {
  return (PRIMARY_IMU == IMU_PRIMARY_BMI) ? (!bmi_data_ok) : (!icm_data_ok);
}

static void force_failover_if_primary_dead() {
  if (!primary_dead()) return;

  if (PRIMARY_IMU == IMU_PRIMARY_BMI) {
    if (icm_data_ok) {
      PRIMARY_IMU = IMU_PRIMARY_ICM;
      last_failover_ms = millis();
      mismatch_counter = 0;
      imu_mismatch = false;
    }
  } else {
    if (bmi_data_ok) {
      PRIMARY_IMU = IMU_PRIMARY_BMI;
      last_failover_ms = millis();
      mismatch_counter = 0;
      imu_mismatch = false;
    }
  }
}

static void maybe_failover_on_mismatch() {
  if (!imu_mismatch) return;
  uint32_t now = millis();
  if (now - last_failover_ms < FAILOVER_COOLDOWN_MS) return;

  if (PRIMARY_IMU == IMU_PRIMARY_BMI) {
    if (icm_data_ok) {
      PRIMARY_IMU = IMU_PRIMARY_ICM;
      last_failover_ms = now;
      mismatch_counter = 0;
      imu_mismatch = false;
    }
  } else {
    if (bmi_data_ok) {
      PRIMARY_IMU = IMU_PRIMARY_BMI;
      last_failover_ms = now;
      mismatch_counter = 0;
      imu_mismatch = false;
    }
  }
}

// =====================================================
// Serial parser for F commands (expects '\n')
// =====================================================
static void parse_serial_fault_cmd() {
  static char buf[48];
  static uint8_t idx=0;

  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;

    if (c == '\n') {
      buf[idx] = '\0';
      idx = 0;

      if (buf[0] == 'F' || buf[0] == 'f') {
        int mode = atoi(buf + 1);
        int dur  = 0;

        char *comma = strchr(buf, ',');
        if (comma) dur = atoi(comma + 1);

        if (mode < 0) mode = 0;
        if (mode > 6) mode = 6;
        if (dur < 0) dur = 0;

        set_fault((FaultMode)mode, (uint32_t)dur);
      }
      continue;
    }

    if (idx < sizeof(buf)-1) buf[idx++] = c;
    else idx = 0;
  }
}

// =====================================================
// Apply fault effects AFTER reading sensors
// =====================================================
static void apply_faults() {
  static bool bmi_stuck_init=false, icm_stuck_init=false;
  static float bmi_ax_s,bmi_ay_s,bmi_az_s,bmi_gx_s,bmi_gy_s,bmi_gz_s;
  static float icm_ax_s,icm_ay_s,icm_az_s,icm_gx_s,icm_gy_s,icm_gz_s;

  switch (faultMode) {
    case FAULT_NONE:
      bmi_stuck_init=false;
      icm_stuck_init=false;
      break;

    case FAULT_BMI_DEAD:
      bmi_data_ok = false;
      break;

    case FAULT_ICM_DEAD:
      icm_data_ok = false;
      break;

    case FAULT_BMI_CORRUPT:
      if (bmi_data_ok) { bmi_ax += 0.80f; bmi_ay -= 0.60f; }
      break;

    case FAULT_ICM_CORRUPT:
      if (icm_data_ok) { icm_ax -= 0.75f; icm_ay += 0.55f; }
      break;

    case FAULT_BMI_STUCK:
      if (bmi_data_ok) {
        if (!bmi_stuck_init) {
          bmi_ax_s=bmi_ax; bmi_ay_s=bmi_ay; bmi_az_s=bmi_az;
          bmi_gx_s=bmi_gx; bmi_gy_s=bmi_gy; bmi_gz_s=bmi_gz;
          bmi_stuck_init=true;
        }
        bmi_ax=bmi_ax_s; bmi_ay=bmi_ay_s; bmi_az=bmi_az_s;
        bmi_gx=bmi_gx_s; bmi_gy=bmi_gy_s; bmi_gz=bmi_gz_s;
      }
      break;

    case FAULT_ICM_STUCK:
      if (icm_data_ok) {
        if (!icm_stuck_init) {
          icm_ax_s=icm_ax; icm_ay_s=icm_ay; icm_az_s=icm_az;
          icm_gx_s=icm_gx; icm_gy_s=icm_gy; icm_gz_s=icm_gz;
          icm_stuck_init=true;
        }
        icm_ax=icm_ax_s; icm_ay=icm_ay_s; icm_az=icm_az_s;
        icm_gx=icm_gx_s; icm_gy=icm_gy_s; icm_gz=icm_gz_s;
      }
      break;
  }
}

// =====================================================
// Setup / Loop
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  imu_init();
  t_prev_us = micros();
}

void loop() {
  parse_serial_fault_cmd();
  fault_tick_auto_clear();

  uint32_t t_now = micros();
  float dt = (t_now - t_prev_us) * 1e-6f;
  if (dt <= 0) dt = 0.005f;
  if (dt > 0.05f) dt = 0.005f;
  t_prev_us = t_now;

  imu_update();
  apply_faults();

  // Failover inmediato si primaria muere
  force_failover_if_primary_dead();

  float bmi_roll_acc  = rad2deg(atan2f(bmi_ay, sqrtf(bmi_ax*bmi_ax + bmi_az*bmi_az)));
  float bmi_pitch_acc = -rad2deg(atan2f(bmi_ax, sqrtf(bmi_ay*bmi_ay + bmi_az*bmi_az)));

  float icm_roll_acc  = rad2deg(atan2f(icm_ay, sqrtf(icm_ax*icm_ax + icm_az*icm_az)));
  float icm_pitch_acc = -rad2deg(atan2f(icm_ax, sqrtf(icm_ay*icm_ay + icm_az*icm_az)));

  if (bmi_data_ok) {
    bmi_roll  = ALPHA*(bmi_roll  + bmi_gx*dt) + (1.0f-ALPHA)*bmi_roll_acc;
    bmi_pitch = ALPHA*(bmi_pitch + bmi_gy*dt) + (1.0f-ALPHA)*bmi_pitch_acc;
  }
  if (icm_data_ok) {
    icm_roll  = ALPHA*(icm_roll  + icm_gx*dt) + (1.0f-ALPHA)*icm_roll_acc;
    icm_pitch = ALPHA*(icm_pitch + icm_gy*dt) + (1.0f-ALPHA)*icm_pitch_acc;
  }

float dRoll  = bmi_roll  - icm_roll;
float dPitch = bmi_pitch - icm_pitch;

  bool dyn = is_dynamic_motion();
  if (!dyn) {
    mismatch_update(dRoll, dPitch);
  } else {
    if (mismatch_counter > 0) mismatch_counter--;
    imu_mismatch = (mismatch_counter >= MISMATCH_HITS);
  }

  maybe_failover_on_mismatch();

  float rollP  = (PRIMARY_IMU == IMU_PRIMARY_BMI) ? bmi_roll  : icm_roll;
  float pitchP = (PRIMARY_IMU == IMU_PRIMARY_BMI) ? bmi_pitch : icm_pitch;

  static uint32_t t_print=0;
  if (millis() - t_print >= 10) {
    t_print = millis();
    Serial.print("P=");
    Serial.print(PRIMARY_IMU == IMU_PRIMARY_BMI ? "BMI" : "ICM");
    Serial.print("  ");

    Serial.print("BMI("); Serial.print(bmi_data_ok); Serial.print(") ");
    Serial.print("r,p="); Serial.print(bmi_roll,1); Serial.print(",");
    Serial.print(bmi_pitch,1);

    Serial.print("  ||  ");

    Serial.print("ICM("); Serial.print(icm_data_ok); Serial.print(") ");
    Serial.print("r,p="); Serial.print(icm_roll,1); Serial.print(",");
    Serial.print(icm_pitch,1);

    Serial.print("  ||  ");
    Serial.print("d="); Serial.print(dRoll,1); Serial.print(",");
    Serial.print(dPitch,1);

    Serial.print("  ||  ");
    Serial.print("MISMATCH="); Serial.print(imu_mismatch ? "YES" : "NO");
    Serial.print(" (cnt="); Serial.print(mismatch_counter); Serial.print(")");

    Serial.println(); 
  }
}

