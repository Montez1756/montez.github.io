#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QScreen>
#include <QGuiApplication>
#include <curl/curl.h>
#include "download.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <QPushButton>

DownloadWidget::DownloadWidget(QWidget *parent = nullptr, const std::string url) : QWidget(parent)
{

    parent->layout()->addWidget(this);
    setGeometry(0, 0, 300, 300);
    setStyleSheet("background-color: dimgrey; QLabel{background-color:grey; color:white;}");
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    layout = new QVBoxLayout(this);
    urlLabel = new QLabel(QString::fromStdString(url), this);
    statusLabel = new QLabel("Starting download...", this);
    progressBar = new QProgressBar(this);

    progressBar->setStyleSheet("background-color:blue;border:1px solid white;");
    layout->addWidget(urlLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(progressBar);

    progressBar->setRange(0, 100);

    startDownload(url);
}

void DownloadWidget::startDownload(const std::string &url)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return;

    // Extract filename from URL
    size_t pos = url.rfind("/");
    std::string filename = (pos != std::string::npos) ? url.substr(pos + 1) : "downloaded_file";

    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        qWarning() << "Failed to open file for writing:" << QString::fromStdString(filename);
        return;
    }

    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    // Set progress callback
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress

    // Perform download in a separate thread
    QtConcurrent::run([=]()
                      {
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        
        QMetaObject::invokeMethod(this, [=]() {
            if (res == CURLE_OK) {
                statusLabel->setText("Download completed: " + QString::fromStdString(filename));
            } else {
                handleDownloadFailure(res, filename);
            }
        }); });
}

int DownloadWidget::progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t)
{
    auto *self = static_cast<DownloadWidget *>(clientp);
    if (dltotal > 0)
    {
        int progress = static_cast<int>((dlnow * 100) / dltotal);
        QMetaObject::invokeMethod(self, [=]()
                                  {
            self->progressBar->setValue(progress);
            self->statusLabel->setText(QString("Downloaded %1 / %2 bytes").arg(dlnow).arg(dltotal)); });
    }
    return 0; // Continue the download
}

void DownloadWidget::handleDownloadFailure(CURLcode res, const std::string &filename)
{
    statusLabel->setText("Download failed: " + QString(curl_easy_strerror(res)));

    // Create Retry and Cancel buttons
    QPushButton *retryButton = new QPushButton("Retry", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    // Add the buttons to the layout
    layout->addWidget(retryButton);
    layout->addWidget(cancelButton);

    // Retry button logic
    connect(retryButton, &QPushButton::clicked, this, [this, filename, &retryButton, &cancelButton]()
            {
                // Logic to retry the download
                layout->removeWidget(retryButton);
                layout->removeWidget(cancelButton);
                retryButton->deleteLater();
                cancelButton->deleteLater();
                startDownload(filename); // Assuming startDownload is a method in your class
            });

    // Cancel button logic
    connect(cancelButton, &QPushButton::clicked, this, [this, &retryButton, &cancelButton]()
            {
        // Cancel the operation (or exit the app)
        layout->removeWidget(retryButton);
        layout->removeWidget(cancelButton);
        retryButton->deleteLater();
        cancelButton->deleteLater();
        statusLabel->setText("Download canceled.");
        hide();
        this->deleteLater(); });

    // Optional: Delete the failed file if it exists
    QFile file(QString::fromStdString(filename));
    if (file.exists())
    {
        if (file.remove())
        {
            statusLabel->setText("Failed file deleted.");
        }
        else
        {
            statusLabel->setText("Failed file could not be deleted.");
        }
    }
}