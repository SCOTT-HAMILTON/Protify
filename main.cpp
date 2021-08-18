#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <QLocale>
#include <QTranslator>

#include "backend.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QQuickStyle::setStyle("Material");
    QCoreApplication::setOrganizationName("Sample");
    QCoreApplication::setOrganizationDomain("sample.com");
    QCoreApplication::setApplicationName("Protify");
    QGuiApplication app(argc, argv);

    QTranslator translator;
    QLocale locale;//(QLocale::English, QLocale::UnitedStates);
    qDebug() << "locale : " << locale.name();

    if (translator.load(":/translations/Protify_"+locale.name()+".qm")) {
        qDebug() << "Successfully loaded :/translations/Protify_"+locale.name()+".qm";
        app.installTranslator(&translator);
    }
    else {
        qDebug() << "Couldn't load :/translations/Protify_" + locale.name() + ".qm";
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    auto backend = new Backend(engine.rootContext());
    engine.rootContext()->setContextProperty("backend", backend);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    backend->startCommunications();

    return app.exec();
}
