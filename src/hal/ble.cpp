#include "ble.h"
#include "utils/queue.h"
#include "printer.h"

BLECharacteristic* pCharacteristic;   // 特征，用于发布数据

void ble_report(void)
{
  if(get_device_state()->connection == true)
  {
    uint8_t buffer[4];
    device_state_struct* device_state = get_device_state();
    buffer[0] = device_state->battery;
    buffer[1] = device_state->temperature;
    buffer[2] = device_state->paper_state;
    buffer[3] = device_state->printer_state;
    pCharacteristic->setValue(buffer, sizeof(buffer));
    pCharacteristic->notify();
  }
}

class bleServerCallback : public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    get_device_state()->connection = true;
    Serial.printf("Device connected\n");
  }

  void onDisconnect(BLEServer* pServer)
  {
    get_device_state()->connection = false;
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
    Serial.printf("Client is on write, length = %d \n", length);
    // 数据包解析
    if(length == 5)
    {
        if(data[0] == 0xA5 && data[1] == 0xA5 && data[2] == 0xA5 && data[3] == 0xA5)
        {
            if(data[4] == 1)
                set_heat_density(30);
            else if(data[4] == 2)
                set_heat_density(60);
            else
                set_heat_density(100);
            return;
        }
        else if(data[0] == 0xA6 && data[1] == 0xA6 && data[2] == 0xA6 && data[3] == 0xA6)
        {
            set_ble_write_finished(true);
            return;
        }
    }
    // 将数据写入缓冲区
    write_oneline_data_to_queue(data, length);
    Serial.println("data was written in queue successfully!");
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
                                | BLECharacteristic::PROPERTY_NOTIFY 
                                | BLECharacteristic::PROPERTY_WRITE_NR);
  // 绑定特征回调函数(class)
  pCharacteristic->setCallbacks(new bleCharacteristicCallback());
  // 启动服务
  pService->start();
  // 设备发布数据（广播）
  BLEDevice::startAdvertising();

}