#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>
#include <QDebug>
#include <QFileDialog>
#include <QFontDatabase>
#include <iostream>
using std::cout;
using std::endl;
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _defaultMusicPath(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).last()),
    _fileSystemModel(new QFileSystemModel()) {

    ui->setupUi(this);
    this->setWindowTitle("AndroidMusic");

    int id = QFontDatabase::addApplicationFont("://resources/fonts/josefin/JosefinSans-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont josefin(family);

    this->setFont(josefin);

    connect(this->ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(this->ui->actionOpen_Playlist, SIGNAL(triggered(bool)), this, SLOT(openPlaylist()));
    connect(this->ui->actionOpen_Directory, SIGNAL(triggered(bool)), this, SLOT(changeDirectory()));
    connect(this->ui->addButton, SIGNAL(clicked(bool)), this, SLOT(addEntry()));
    connect(this->ui->removeButton, SIGNAL(clicked(bool)), this, SLOT(removeEntry()));
    connect(this->ui->syncButton, SIGNAL(clicked(bool)), this, SLOT(sync()));
    connect(this->ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(savePlaylist()));

    QStringList audioFileTypes;
    audioFileTypes << "*.3gp" << "*.aa" << "*.aac" << "*.aax" << "*.act";
    audioFileTypes << "*.aiff" << "*.amr" << "*.ape" << "*.au" << "*.awb";
    audioFileTypes << "*.dct" << "*.dss" << "*.dvf" << "*.flac" << "*.gsm";
    audioFileTypes << "*.iklax" << "*.ivs" << "*.m4a" << "*.m4b" << "*.m4p";
    audioFileTypes << "*.mmf" << "*.mp3" << "*.mpc" << "*.msv" << "*.ogg";
    audioFileTypes << "*.opus" << "*.ra" << "*.raw" << "*.sln" << "*.tta";
    audioFileTypes << "*.vox" << "*.wav" << "*.wma" << "*.wv" << "*.webm" << "*.mp4";

    this->_fileSystemModel->setRootPath(this->_defaultMusicPath);
    this->_fileSystemModel->setNameFilters(audioFileTypes);
    this->ui->fileSystemView->setModel(this->_fileSystemModel);
    this->ui->fileSystemView->setRootIndex(this->_fileSystemModel->index(this->_defaultMusicPath));

    for(auto i = 1; i < this->_fileSystemModel->columnCount(); i++)
        this->ui->fileSystemView->hideColumn(i);

    this->ui->centralWidget->setLayout(this->ui->mainLayout);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openPlaylist() {

}

void MainWindow::changeDirectory() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setDirectory(this->_defaultMusicPath);
    dialog.setWindowTitle("Open Directory");

    if(dialog.exec() == QDialog::Accepted) {
        QString dir = dialog.selectedFiles().first();
        this->_fileSystemModel->setRootPath(dir);
        this->ui->fileSystemView->setRootIndex(this->_fileSystemModel->index(dir));
    }
}

void MainWindow::addEntry() {
    QString filepath = this->_fileSystemModel->filePath(this->ui->fileSystemView->currentIndex());
    auto primPath = filepath.toStdString();
    const char * superPrimPath = primPath.c_str();
    FileRef ref(superPrimPath);

    auto title = ref.tag()->artist() + " - " + ref.tag()->title() + "." + this->getFileExtension(filepath).toStdString();
    auto primTitle = title.toCString();

    if(this->_musicMap.find(QString(primTitle)) == this->_musicMap.end()) {
        this->_musicMap[QString(primTitle)] = filepath;
        this->ui->playlistWidget->addItem(QString(primTitle));
    }
}

void MainWindow::removeEntry() {
    QListWidgetItem *it = this->ui->playlistWidget->takeItem(this->ui->playlistWidget->currentRow());
    this->_musicMap.erase(it->text());
    delete it;
}

void MainWindow::sync() {
    this->_errors.clear();
    this->_messages.clear();
    QString baseCommand = "adb push ";
    for(auto i : this->_musicMap) {
        QString command = baseCommand + "'" + i.second + "'";
        command += " '/sdcard/Music/";
        command += i.first + "'";
        auto response = system(command.toStdString().c_str());

        if(response == 0)
            this->_messages.push_back("Added " + i.second);
        else
            this->_errors.push_back("Error adding " + i.second);
    }
}

void MainWindow::savePlaylist() {

}

QString MainWindow::getFileExtension(const QString &filename) {
    QString ext = "";
    for(auto i = filename.size()-1; i >= 0; i--) {
        if(filename.at(i) == ".")
            break;
        ext = filename.at(i) + ext;
    }

    return ext;
}
