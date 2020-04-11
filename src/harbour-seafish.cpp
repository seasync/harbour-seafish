#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <src/seafile/open-local-helper.h>



namespace {
void initGlib()
{
#if !GLIB_CHECK_VERSION(2, 35, 0)
    g_type_init();
#endif
#if !GLIB_CHECK_VERSION(2, 31, 0)
    g_thread_init(NULL);
#endif
}

void setupSettingDomain()
{
    // see QSettings documentation
    QCoreApplication::setOrganizationName(getBrand());
    QCoreApplication::setOrganizationDomain("seafile.com");
    QCoreApplication::setApplicationName(QString("%1 Client").arg(getBrand()));
}

void handleCommandLineOption(int argc, char *argv[])
{
    int c;
    static const char *short_options = "KDXPc:d:f:";
    static const struct option long_options[] = {
        { "config-dir", required_argument, NULL, 'c' },
        { "data-dir", required_argument, NULL, 'd' },
        { "stop", no_argument, NULL, 'K' },
        { "delay", no_argument, NULL, 'D' },
        { "remove-user-data", no_argument, NULL, 'X' },
        { "open-local-file", no_argument, NULL, 'f' },
        { "stdout", no_argument, NULL, 'l' },
        { "ping", no_argument, NULL, 'P' },
        { NULL, 0, NULL, 0, },
    };

    while ((c = getopt_long (argc, argv, short_options,
                             long_options, NULL)) != EOF) {
        switch (c) {
        case 'c':
            g_setenv ("CCNET_CONF_DIR", optarg, 1);
            break;
        case 'd':
            g_setenv ("SEAFILE_DATA_DIR", optarg, 1);
            break;
        case 'l':
            g_setenv ("LOG_STDOUT", "", 1);
            break;
        case 'K':
            do_stop();
            exit(0);
        case 'P':
            do_ping();
            exit(0);
        case 'D':
            msleep(1000);
            break;
        case 'X':
            do_remove_user_data();
            exit(0);
        case 'f':
            OpenLocalHelper::instance()->handleOpenLocalFromCommandLine(optarg);
            break;
        default:
            exit(1);
        }
    }

}

} // anonymous namespace

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/harbour-seafish.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    initGlib();

    QGuiApplication *app {SailfishApp::application(argc, argv)};
    QQuickView *view {SailfishApp::createView()};

    view->setSource(SailfishApp::pathToMainQml());

    setupSettingDomain();

    handleCommandLineOption(argc, argv);

    if (count_process(APPNAME) > 1) {
            if (OpenLocalHelper::instance()->activateRunningInstance()) {
                printf("Activated running instance of seafile client\n");
                return 0;
            }
            QMessageBox::warning(NULL, getBrand(),
                                 QObject::tr("%1 Client is already running").arg(getBrand()),
                                 QMessageBox::Ok);
            return -1;
        }

    //Start routine
    view->show();

    return app->exec();
}
