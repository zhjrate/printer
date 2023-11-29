#include <Arduino.h>
#include <BLEDevice.h>
#include <BLE2902.h>

#define LED_PIN 18
#define KEY_PIN 5
#define BATTERY_ADC_PIN 34

#define ADC_FILTERING_COUNT 10

// Server -> Service -> Charateristic -> Property
#define DEVICE_NAME "mini-printer"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool ble_connected = false;
BLECharacteristic* pCharacteristic;   // 特征，用于发布数据

void ble_report(void)
{
  if(ble_connected)
  {
    uint8_t buffer[5];
    buffer[0] = 1;
    buffer[1] = 2;
    buffer[2] = 3;
    buffer[3] = 4;
    buffer[4] = 5;
    pCharacteristic->setValue(buffer, sizeof(buffer));
    pCharacteristic->notify();
  }
}

class bleServerCallback : public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    ble_connected = true;
    Serial.printf("Device connected\n");
  }

  void onDisconnect(BLEServer* pServer)
  {
    ble_connected = false;
    Serial.printf("Device disconnected\n");
    // 重启广播
    pServer->startAdvertising();
  }
};

class bleCharacteristicCallback : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic* pCharacteristic)
  {
    Serial.printf("Client is on read.\n");
  }

  void onWrite(BLECharacteristic* pCharacteristic)
  {
    size_t length = pCharacteristic->getLength();
    uint8_t* data = pCharacteristic->getData();
    Serial.printf("Clent is on write, length = %d \n", length);
  }
};

void ble_init(void)
{
  // 初始化蓝牙设备
  BLEDevice::init(DEVICE_NAME);
  // 创建服务器
  BLEServer* pServer = BLEDevice::createServer();
  // 绑定服务器回调函数(class)
  pServer->setCallbacks(new bleServerCallback());
  // 创建服务
  BLEService* pService = pServer->createService(SERVICE_UUID);
  // 创建特征
  pCharacteristic = pService->createCharacteristic( 
                                  CHARACTERISTIC_UUID, 
                                  BLECharacteristic::PROPERTY_READ
                                | BLECharacteristic::PROPERTY_WRITE
                                | BLECharacteristic::PROPERTY_NOTIFY );
  // 绑定特征回调函数(class)
  pCharacteristic->setCallbacks(new bleCharacteristicCallback());
  // 启动服务
  pService->start();
  // 设备发布数据（广播）
  BLEDevice::startAdvertising();

}

void setup()
{
  Serial.begin(9600);
  ble_init();
  Serial.println("setup");
}

void loop() 
{
  delay(5000);
  ble_report();
}

