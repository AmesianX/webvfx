#include <QApplication>
#include <QThread>
#include "webfx/web_image.h"
#include "webfx/web_logger.h"
#include "webfx/web_page.h"
#include "webfx/web_parameters.h"
#include "webfx/web_renderer.h"


namespace WebFX
{

WebRenderer::WebRenderer()
    : QObject(0)
    , webPage(0)
{
}

bool WebRenderer::initialize(const std::string& url, int width, int height, WebParameters* parameters)
{
    QUrl qurl(QString::fromStdString(url));

    if (!qurl.isValid()) {
        log(std::string("Invalid URL: ") + url);
        return false;
    }

    QSize size(width, height);

    if (onUIThread()) {
        initializeInvokable(qurl, size, parameters);
    }
    else {
        // Move ourself onto GUI thread and create our WebPage there
        this->moveToThread(QApplication::instance()->thread());
        QMetaObject::invokeMethod(this, "initializeInvokable", Qt::BlockingQueuedConnection,
                                  Q_ARG(QUrl, qurl), Q_ARG(QSize, size), Q_ARG(WebParameters*, parameters));
    }

    //XXX check actual load status

    return true;
}

void WebRenderer::destroy()
{
    deleteLater();
}

bool WebRenderer::onUIThread() {
    return QThread::currentThread() == QApplication::instance()->thread();
}

const WebEffects::ImageTypeMap& WebRenderer::getImageTypeMap()
{
    return webPage->getImageTypeMap();
}

WebImage WebRenderer::getImage(const std::string& name, int width, int height)
{
    // This may create a QImage and modify QHash - both of those classes are reentrant,
    // so should be safe to do on calling thread as long as access to this WebRenderer is synchronized.
    return webPage->getWebImage(QString::fromStdString(name), QSize(width, height));
}

const WebImage WebRenderer::render(double time, int width, int height)
{
    QSize size(width, height);

    if (onUIThread()) {
        renderInvokable(time, size);
    }
    else {
        QMetaObject::invokeMethod(this, "renderInvokable", Qt::BlockingQueuedConnection,
                                  Q_ARG(double, time), Q_ARG(QSize, size));
    }
    return renderImage;
}

void WebRenderer::initializeInvokable(const QUrl& url, const QSize& size, WebParameters* parameters)
{
    webPage = new WebPage(this, parameters);
    webPage->setViewportSize(size);

    //XXX we should enable webgl for our QtWebKit builds
    webPage->settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    webPage->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);

    // Turn off scrollbars
    webPage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    webPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    //XXX need to get result back to initialize()
    bool result = webPage->loadSync(url);
    // 4.8.0 allows BlockingQueuedConnection to return a value http://bugreports.qt.nokia.com/browse/QTBUG-10440

}

void WebRenderer::renderInvokable(double time, const QSize& size)
{
    webPage->setViewportSize(size);
    renderImage = webPage->render(time);
}

}
