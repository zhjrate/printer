#include "ble.h"

BLECharacteristic* pCharacteristic;   // 特征，用于发布数据
bool ble_write_finished = false;      // 蓝牙打印数据写入完成标志位

// 设置写入完成标志位状态
void set_ble_write_finished(bool finish)
{
    ble_write_finished = finish;
}

// 获取写入完成标志位状态
bool get_ble_write_finished(void)
{
    return ble_write_finished;
}

// 设备状态上报函数
void ble_report(void)
{
  // 如果连接正常，则上报设备状态
  if(get_device_state().connection == true)
  {
    // 将设备状态打包到一个buffer里面进行上报
    uint8_t buffer[4];
    device_state_struct device_state = get_device_state();
    buffer[0] = device_state.battery;
    buffer[1] = device_state.temperature;
    buffer[2] = device_state.paper_state;
    buffer[3] = device_state.printer_state;
    pCharacteristic->setValue(buffer, sizeof(buffer));
    pCharacteristic->notify();
    // 串口调试输出
    Serial.printf("battery = %d",buffer[0]);
    Serial.printf("\ttemperature = %d",buffer[1]);
    Serial.printf("\tpaper state = %d",buffer[2]);
    Serial.printf("\tprinter state = %d",buffer[3]);
    Serial.printf("\n");
  }
}

// 服务器回调函数类
class bleServerCallback : public BLEServerCallbacks
{
  // 连接成功回调函数
  void onConnect(BLEServer* pServer)
  {
    // 将设备状态设置为蓝牙已连接
    get_device_state().connection = true;
    set_led_state(LED_CONNECT);
    Serial.printf("Device connected\n");
  }

  // 断开连接回调函数
  void onDisconnect(BLEServer* pServer)
  {
    // 将设备状态设置为蓝牙已断开
    get_device_state().connection = false;
    set_led_state(LED_DISCONNECT);
    Serial.printf("Device disconnected\n");
    // 重启广播
    pServer->startAdvertising();
  }
};

// 特征函数类
class bleCharacteristicCallback : public BLECharacteristicCallbacks
{
  // 上位机主动读取数据回调
  void onRead(BLECharacteristic* pCharacteristic)
  {
    Serial.printf("Client is on read.\n");
  }

  // 上位机主动写入数据回调
  void onWrite(BLECharacteristic* pCharacteristic)
  {
    size_t length = pCharacteristic->getLength();       // 待写入的字节数
    uint8_t* data = pCharacteristic->getData();         // 待写入的数据
    Serial.printf("Client is on write, length = %d \n", length);
    // 数据包解析
    if(length == 5)
    {
      // 接收数据首部为: A5 A5 A5 A5 XX 时，表示设置打印头的热密度，XX表示热密度数值
      if(data[0] == 0xA5 && data[1] == 0xA5 && data[2] == 0xA5 && data[3] == 0xA5)
      {
        if(data[4] == 1) {
           set_heat_density(30);
           Serial.println("heat density was set to 30%");
        }
        else if(data[4] == 2) {
          set_heat_density(60);
          Serial.println("heat density was set to 60%");
        }
        else {
          set_heat_density(100);
          Serial.println("heat density was set to 100%");
        }
        return;
      }
      // 接收数据首部为: A6 A6 A6 A6 XX时，表示打印数据发送完成
      else if(data[0] == 0xA6 && data[1] == 0xA6 && data[2] == 0xA6 && data[3] == 0xA6)
      {
        set_ble_write_finished(true);
        Serial.println("data write finished!");
        return;
      }
    }
    // 将打印数据写入缓冲区
    write_oneline_data_to_queue(data, length);
    Serial.println("data was written in queue successfully!");
  }
};

// 蓝牙初始化函数
// 初始化过程：Server -> Service -> Charateristic -> Property
void ble_init(void)
{
    // 初始化蓝牙设备
    BLEDevice::init(DEVICE_NAME);
    // BLEDevice::startAdvertising();
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
    // pCharacteristic->addDescriptor(new BLE2902()); // 添加描述 
    // 启动服务
    pService->start();
    // 设备发布数据（广播）
    BLEDevice::startAdvertising();

}