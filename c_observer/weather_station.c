#include <stdio.h>
#include <stdlib.h>
#include "observer.h"

// 天气数据结构
typedef struct {
    float temperature;
    float humidity;
    float pressure;
} WeatherData;

// 观察者函数：显示面板
void displayPanel(void* data) {
    WeatherData* weather = (WeatherData*)data;
    printf("Display Panel:\n");
    printf("Temperature: %.2f°C\n", weather->temperature);
    printf("Humidity: %.2f%%\n", weather->humidity);
    printf("Pressure: %.2f hPa\n\n", weather->pressure);
}

// 观察者函数：警报系统
void alertSystem(void* data) {
    WeatherData* weather = (WeatherData*)data;
    if (weather->temperature > 30.0) {
        printf("Alert System: High temperature warning! %.2f°C\n\n", weather->temperature);
    }
}

// 天气站类
typedef struct {
    Subject* subject;
    WeatherData data;
} WeatherStation;

// 更新天气数据并通知观察者
void setWeatherData(WeatherStation* station, float temperature, float humidity, float pressure) {
    station->data.temperature = temperature;
    station->data.humidity = humidity;
    station->data.pressure = pressure;
    notify(station->subject, &station->data);
}

int main() {
    WeatherStation station;
    station.subject = createSubject();

    // 注册观察者
    attach(station.subject, displayPanel);
    attach(station.subject, alertSystem);

    // 更新天气数据
    setWeatherData(&station, 25.0, 60.0, 1013.0);
    setWeatherData(&station, 32.0, 70.0, 1010.0);
    setWeatherData(&station, 28.0, 50.0, 1005.0);

    // 注销观察者
    detach(station.subject, displayPanel);

    // 更新天气数据
    setWeatherData(&station, 35.0, 65.0, 1000.0);

    // 清理资源
    destroySubject(station.subject);

    return 0;
}