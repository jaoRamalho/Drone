#include "Control.hpp"

volatile bool Control::buttonPressed = false;
volatile StateSystem Control::systemState = IDLE;
volatile uint32_t Control::timeButton = 0;
Control *Control::instance = nullptr;
volatile uint32_t Control::timeState = 0;
uint8_t Control::ledState = 0;
volatile uint32_t Control::timeLed = 250;
volatile uint32_t Control::timeBattery = 0;
volatile uint32_t Control::accelerationTime = 0;


float dt = (float)TIME_LOOP_FLYING / 1000.0f; // em segundos

Control::Control() : setupTimeLED(250), valueBattery(0) {
    Serial.println("| CONTROL | ---------- Iniciando Control --------");

    pinMode(LED, OUTPUT);
    digitalWrite(LED, ledState);

    gyro = Gyroscope::Init_Gyroscope();

    Serial.println("| CONTROL | ---------- Control Iniciado --------");

    state = WAKEUP_ALL;
    previousState = state;
}

Control::~Control(){

}

void Control::checkBattery(){
    if (timeBattery == 0){
        timeBattery = 1000;
        valueBattery = (uint8_t)(analogRead(V_BAT_PIN) / 10.53);
        Serial.print("| CONTROL | - Nível da bateria: ");
        Serial.print(valueBattery);
    }
}

Control *Control::Init_Control()
{
    if (instance == nullptr){
        instance = new Control();
    }
    return instance;
}

uint8_t commonValueServants = 1;
void Control::checkTimeButton(uint32_t deltaTime){
    if(deltaTime > 100) {
        if (systemState == IDLE) {
            systemState = START;
        } else if (systemState == START) {
            systemState = IDLE;
            movement.offMotors();
            timeState = 0;
            state = WAKEUP_ALL;
            previousState = state;
            movement.resetPIDValues();
            commonValueServants = 0;
        } 
    }
}


void Control::loopMotors(){

    switch (state){
        case WAKEUP_ALL: {
            Serial.println("| CONTROL | - Estado WAKEUP");
            timeState = 10;
            movement.setPercentVelocityMotor(1, MOTOR_ALL);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            setupTimeLED = 100;
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= 25) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    setupTimeLED = 600;
                    timeState = 1000000;
                    commonValueServants = 0;
                    return;
                }
                else {
                    commonValueServants += 1;
                    movement.setPercentVelocityMotor(commonValueServants, MOTOR_ALL);
                    timeState = 600;
                }
            }
            break;
        }
        case ARMED_ALL: {
            Serial.println("| CONTROL | - Estado ARMED");
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            break;
        }
        case FLYING: {
            if(timeState == 0) {
                timeState = TIME_LOOP_FLYING;
            
                movement.computeInclination();
                
                movement.PID_Pitch(dt);
                movement.PID_Roll(dt);
                movement.PID_Yaw(dt);

                movement.ApllyEffectsMotors();
            }

            if (!accelerationTime) {
                movement.loopAcceleration();
                accelerationTime = 1; // atualiza a cada 1 ms
            }
            break;
        }
        default:
            break;
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
        loopConfig();
        checkBattery();
        break;
    case STOP:
        movement.offMotors();
        break;
    default:
        break;
    }
}

void Control::setMovementCommand(MoveCommand cmd)
{
    movement.setCommand(cmd);
    // Serial.print("| CONTROL | - Comando recebido: ");
    // Serial.println(static_cast<int>(cmd));

     switch (cmd) {
        case MoveCommand::UP :
            systemState = START; 
            Serial.println("SUBINDO");
            break;

        case MoveCommand::STOP : 
            systemState = IDLE;
            movement.offMotors();
            timeState = 10;
            state = WAKEUP_ALL;
            previousState = state;
            movement.resetPIDValues();
            commonValueServants = 0; 
            Serial.println("PARANDO");   
            break;

        case MoveCommand::NONE :
        default:
            break;
    }
}

