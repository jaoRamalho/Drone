#include "Control.hpp"

volatile bool Control::buttonPressed = false;
volatile StateSystem Control::systemState = IDLE;
volatile uint32_t Control::timeButton = 0;
Control *Control::instance = nullptr;
uint32_t Control::timeState = 0;
uint8_t Control::ledState = 0;
uint32_t Control::timeLed = 250;
volatile uint32_t Control::timeBattery = 0;
Control::Control() : commonValueServants(0), valueM1(0), valueM2(0), valueM3(0), valueM4(0),
setupTimeLED(250), valueBattery(0)
{
    Serial.println("| CONTROL | ---------- Iniciando Control --------");

    pinMode(LED, OUTPUT);
    digitalWrite(LED, ledState);

    gyro = Gyroscope::Init_Gyroscope();

    Serial.println("| CONTROL | ---------- Control Iniciado --------");
}

Control::~Control()
{
}

void Control::resetMotorsValues()
{
    valueM1 = commonValueServants;
    valueM2 = commonValueServants;
    valueM3 = commonValueServants;
    valueM4 = commonValueServants;
}

void Control::checkBattery(){
    if (timeBattery == 0){
        timeBattery = 1000;
        valueBattery = (uint8_t)(analogRead(V_BAT_PIN) / 10.53);
        Serial.println("| CONTROL | - Nível da bateria: " + String(valueBattery) + "%");
    }
}

void Control::setupMotors(){

    pinMode(PIN_M1, OUTPUT);
    m1.attach(PIN_M1);
    m1.write(commonValueServants);

    pinMode(PIN_M2, OUTPUT);
    m2.attach(PIN_M2);
    m2.write(commonValueServants);

    pinMode(PIN_M3, OUTPUT);
    m3.attach(PIN_M3);
    m3.write(commonValueServants);

    pinMode(PIN_M4, OUTPUT);
    m4.attach(PIN_M4);
    m4.write(commonValueServants);

    state = WAKEUP_ALL;
    previousState = state;
}

uint8_t Control::convertPercentForPeriod(uint8_t percent){
    return OFFSET_MOTORS + ((MAX_MOTORS - OFFSET_MOTORS) * percent) / 100;;
}

void Control::setPercentVelocityMotor(uint8_t percent, MotorNumber motorNumber){
    uint8_t valueToSet = Control::convertPercentForPeriod(percent);
    switch (motorNumber){
        case MOTOR_1:
            valueM1 = valueToSet;
            m1.write(valueM1);
            break;
        case MOTOR_2:
            valueM2 = valueToSet;
            m2.write(valueM2);
            break;
        case MOTOR_3:
            valueM3 = valueToSet;
            m3.write(valueM3);
            break;
        case MOTOR_4:
            valueM4 = valueToSet;
            m4.write(valueM4);
            break;
        case MOTOR_ALL:
            valueM1 = valueToSet;
            m1.write(valueM1);
            valueM2 = valueToSet;
            m2.write(valueM2);
            valueM3 = valueToSet;
            m3.write(valueM3);
            valueM4 = valueToSet;
            m4.write(valueM4);
            commonValueServants = valueToSet;
            break;
        default:
            break;
    }
}

void Control::offMotors(){
    m1.detach();
    m2.detach();
    m3.detach();
    m4.detach();

    // garante pinos em LOW (evita saída flutuante)
    pinMode(PIN_M1, OUTPUT);
    digitalWrite(PIN_M1, LOW);

    pinMode(PIN_M2, OUTPUT);
    digitalWrite(PIN_M2, LOW);

    pinMode(PIN_M3, OUTPUT);
    digitalWrite(PIN_M3, LOW);

    pinMode(PIN_M4, OUTPUT);
    digitalWrite(PIN_M4, LOW);

    // zera valores internos
    commonValueServants = 0;
    valueM1 = valueM2 = valueM3 = valueM4 = 0;
}

Control *Control::Init_Control()
{
    if (instance == nullptr){
        instance = new Control();
    }
    return instance;
}

void Control::checkTimeButton(uint32_t deltaTime){
    if(deltaTime > 100) {
        if (systemState == IDLE) {
            Serial.println("| CONTROL | - Entrando em START");
            systemState = START;
            setupMotors();
        } else if (systemState == START) {
            Serial.println("| CONTROL | - Entrando em STOP");
            systemState = IDLE;
            offMotors();
        } 
    }
}

void Control::loopMotors()
{
    switch (state){
        case WAKEUP_ALL: {
            Serial.println("| CONTROL | - Estado WAKEUP");
            timeState = 5000;
            setPercentVelocityMotor(0, MOTOR_ALL);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            break;
        }
        case WAKEUP_M1: {
            Serial.println("| CONTROL | - Estado WAKEUP_M1");
            setPercentVelocityMotor(0, MOTOR_1);
            timeState = 5000;
            previousState = state;
            state = ARMED_M1;
            Serial.println("| CONTROL | - Set Estado ARMED_M1");
            break;
        }
        case WAKEUP_M2: {
            Serial.println("| CONTROL | - Estado WAKEUP_M2");
            setPercentVelocityMotor(0, MOTOR_2);
            timeState = 5000;
            previousState = state;
            state = ARMED_M2;
            Serial.println("| CONTROL | - Set Estado ARMED_M2");
            break;
        }
        case WAKEUP_M3: {
            Serial.println("| CONTROL | - Estado WAKEUP_M3");
            setPercentVelocityMotor(0, MOTOR_3);
            timeState = 5000;
            previousState = state;
            state = ARMED_M3;
            Serial.println("| CONTROL | - Set Estado ARMED_M3");
            break;
        }
        case WAKEUP_M4: {
            Serial.println("| CONTROL | - Estado WAKEUP_M4");
            setPercentVelocityMotor(0, MOTOR_4);
            timeState = 5000;
            previousState = state;
            state = ARMED_M4;
            Serial.println("| CONTROL | - Set Estado ARMED_M4");
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= 80) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    timeState = 10000;
                }
                else {
                    commonValueServants += 5;
                    setPercentVelocityMotor(commonValueServants, MOTOR_ALL);
                    timeState = 1000;
                }
                previousState = state;
                state = ARMED_ALL;
            }
            break;
        }
        case ARMED_ALL: {
            Serial.println("| CONTROL | - Estado ARMED");
            setPercentVelocityMotor(commonValueServants, MOTOR_ALL);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            break;
        }
        case ARMED_M1: {
            Serial.println("| CONTROL | - Estado ARMED_M1");
            setPercentVelocityMotor(commonValueServants, MOTOR_1);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M2: {
            Serial.println("| CONTROL | - Estado ARMED_M2");
            setPercentVelocityMotor(commonValueServants, MOTOR_2);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M3: {
            Serial.println("| CONTROL | - Estado ARMED_M3");
            setPercentVelocityMotor(commonValueServants, MOTOR_3);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M4: {
            Serial.println("| CONTROL | - Estado ARMED_M4");
            setPercentVelocityMotor(commonValueServants, MOTOR_4);
            previousState = state;
            state = FLYING;
            break;
        }
        case FLYING: {
            if (!timeState) {
                commonValueServants -= 5;
                resetMotorsValues();
                if (commonValueServants <= 0){
                    commonValueServants = 0;
                    state = WAKEUP_ALL;
                    timeState = 5000;
                }
                else{
                    timeState = 1000;
                }
                setPercentVelocityMotor(commonValueServants, MOTOR_ALL);
            }
            break;
        }
    }
}

void Control::loopConfig(){
    if(buttonPressed){
        digitalWrite(LED, LOW);
    } else if(!timeLed){
        ledState = !ledState;
        digitalWrite(LED, ledState);
        timeLed = setupTimeLED;
    }
}

void Control::loop(){
    switch (systemState){
    case IDLE:
        loopConfig();
        checkBattery();
        break;
    case START:
        gyro->loop();
        loopMotors();
        uint8_t m1, m2, m3, m4;
        movement.computeMotors(m1, m2, m3, m4);
        loopConfig();
        checkBattery();
        break;
    case STOP:
        offMotors();
        break;
    default:
        break;
    }
}

void Control::setMovementCommand(MoveCommand cmd)
{
    movement.setCommand(cmd);
}

