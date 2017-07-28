#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QListView>
#include <taglib/fileref.h>
using TagLib::FileRef;
#include <map>
using std::map;
#include <vector>
using std::vector;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void openPlaylist();
    void changeDirectory();
    void addEntry();
    void removeEntry();
    void sync();
    void savePlaylist();

private:
    Ui::MainWindow *ui;

    QString _defaultMusicPath;
    QFileSystemModel * _fileSystemModel;

    map<QString, QString> _musicMap;
    vector<QString> _errors;
    vector<QString> _messages;

    QString getFileExtension(const QString & filename);
};

#endif // MAINWINDOW_H
