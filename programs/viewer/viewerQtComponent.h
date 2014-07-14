#ifndef _viewerQtComponent_h
#define _viewerQtComponent_h

#include <cisstMultiTask/mtsComponent.h>

#include <QMainWindow>
#include <QObject>

#include "ui_viewerQtWidget.h"
#include "jsCppBridge.h"

class viewerQtComponent : public QObject, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    viewerQtComponent(const std::string & componentName);
    ~viewerQtComponent(void);

    void Configure(const std::string & CMN_UNUSED(filename)) {};

protected:
#if 0
    void EventVoidHandler(void);
    void EventWriteHandler(const mtsInt & value);
#endif

    Ui::viewerQtWidget viewerWidget;
    QDockWidget CentralWidget;
    QMainWindow MainWindow;
    QJsCppBridge * Bridge;

#if 0
    mtsFunctionVoid Void;
    mtsFunctionWrite Write;
    mtsFunctionRead Read;
    mtsFunctionQualifiedRead QualifiedRead;

    int EventCounter;

signals:
    void ReadQSignal(int newValue);
    void QualifiedReadQSignal(int newValue);
    void EventQSignal(int newValue);

    public slots:
        void VoidQSlot(void);
    void WriteQSlot(int newValue);
    void ReadQSlot(void);
    void QualifiedReadQSlot(int newValue);
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(viewerQtComponent);

#endif  // _viewerQtComponent_h
