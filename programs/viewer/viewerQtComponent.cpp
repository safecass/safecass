#include "viewerQtComponent.h"
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "config.h"
#include "QtJsBridge/jsCppBridge.h"

#include <QUrl>
#include <QWebFrame>

CMN_IMPLEMENT_SERVICES(viewerQtComponent);

viewerQtComponent::viewerQtComponent(const std::string & componentName):
    mtsComponent(componentName)
{
    /*
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * required = AddInterfaceRequired("Required");
    if (required) {
        required->AddFunction("Void", Void);
        required->AddFunction("Write", Write);
        required->AddFunction("Read", Read);
        required->AddFunction("QualifiedRead", QualifiedRead);
        required->AddEventHandlerVoid(&viewerQtComponent::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&viewerQtComponent::EventWriteHandler, this, "EventWrite");
    }
    */

    viewerWidget.setupUi(&CentralWidget);

    std::stringstream url;
    url << "file://" << SF_SOURCE_ROOT_DIR << "/programs/viewer/wheel.html";
    QUrl startURL = QUrl(url.str().c_str());
    viewerWidget.webView->setUrl(startURL);

    QWebFrame * frame = viewerWidget.webView->page()->mainFrame();
    Bridge = new QJsCppBridge(frame);

    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.setWindowTitle(QString::fromStdString(componentName));
    MainWindow.show();

    frame->evaluateJavaScript("alert('test');");
    //Bridge->JsApiProxy->OnEvent("aaa bbb cc dd");

    /*
    // trigger void command
    QObject::connect(viewerWidget.VoidButton, SIGNAL(clicked()),
                     this, SLOT(VoidQSlot()));
    // trigger write command
    QObject::connect(viewerWidget.WriteSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(WriteQSlot(int)));
    // trigger read command and then refresh the UI
    QObject::connect(viewerWidget.ReadButton, SIGNAL(clicked()),
                     this, SLOT(ReadQSlot()));
    QObject::connect(this, SIGNAL(ReadQSignal(int)),
                     viewerWidget.ReadValue, SLOT(setNum(int)));
    // trigger qualified read command and then refresh the UI
    QObject::connect(viewerWidget.QualifiedReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(QualifiedReadQSlot(int)));
    QObject::connect(this, SIGNAL(QualifiedReadQSignal(int)),
                     viewerWidget.QualifiedReadReadValue, SLOT(setNum(int)));
    // refresh event counter when events are received
    QObject::connect(this, SIGNAL(EventQSignal(int)),
                     viewerWidget.EventValue, SLOT(setNum(int)));
                     */
}

viewerQtComponent::~viewerQtComponent()
{
    delete Bridge;
}

#if 0
void viewerQtComponent::EventVoidHandler(void)
{
    EventCounter = 0;
    //emit EventQSignal(EventCounter);
}


void viewerQtComponent::EventWriteHandler(const mtsInt & value)
{
    EventCounter += value.Data;
    //emit EventQSignal(EventCounter);
}


void viewerQtComponent::VoidQSlot(void)
{
    Void();
}


void viewerQtComponent::WriteQSlot(int newValue)
{
    mtsInt data = newValue;
    Write(data);
}


void viewerQtComponent::ReadQSlot(void)
{
    mtsInt data;
    Read(data);
    emit ReadQSignal(data.Data);
}


void viewerQtComponent::QualifiedReadQSlot(int newValue)
{
    mtsInt qualifier, data;
    qualifier.Data = newValue;
    QualifiedRead(qualifier, data);
    emit QualifiedReadQSignal(data.Data);
}
#endif
