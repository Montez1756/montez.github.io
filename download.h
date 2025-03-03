#ifndef DOWNLOAD_H
#define DOWNLOAD_H
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    DownloadWidget(QWidget *parent = nullptr, const std::string url);

private:
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QLabel *urlLabel;
    QVBoxLayout *layout;
    void startDownload(const std::string &url);

    static int progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t);
    void handleDownloadFailure(CURLcode res, const std::string &filename);
};

#endif