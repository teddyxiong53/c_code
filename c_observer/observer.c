#include <stdlib.h>
#include "observer.h"

Subject* createSubject() {
    Subject* subject = (Subject*)malloc(sizeof(Subject));
    subject->observers = NULL;
    return subject;
}

void attach(Subject* subject, ObserverCallback callback) {
    Observer* observer = (Observer*)malloc(sizeof(Observer));
    observer->callback = callback;
    observer->next = subject->observers;
    subject->observers = observer;
}

void detach(Subject* subject, ObserverCallback callback) {
    Observer* current = subject->observers;
    Observer* previous = NULL;

    while (current != NULL) {
        if (current->callback == callback) {
            if (previous == NULL) {
                subject->observers = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void notify(Subject* subject, void* data) {
    Observer* current = subject->observers;
    while (current != NULL) {
        current->callback(data);
        current = current->next;
    }
}

void destroySubject(Subject* subject) {
    Observer* current = subject->observers;
    while (current != NULL) {
        Observer* temp = current;
        current = current->next;
        free(temp);
    }
    free(subject);
}