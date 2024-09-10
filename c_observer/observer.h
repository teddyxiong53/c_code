#ifndef OBSERVER_H
#define OBSERVER_H

typedef void (*ObserverCallback)(void* data);

typedef struct Observer {
    ObserverCallback callback;
    struct Observer* next;
} Observer;

typedef struct Subject {
    Observer* observers;
} Subject;

// 创建主题
Subject* createSubject();

// 注册观察者
void attach(Subject* subject, ObserverCallback callback);

// 注销观察者
void detach(Subject* subject, ObserverCallback callback);

// 通知所有观察者
void notify(Subject* subject, void* data);

// 释放主题
void destroySubject(Subject* subject);

#endif // OBSERVER_H