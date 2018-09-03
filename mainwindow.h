#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CDatabaseManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void queryButtonClicked();
    void ftsQueryButtonClicked();

    void createTestData();
    int createTestData(quint64 rowCount, bool isVirtual);

private:
    Ui::MainWindow *ui;
    CDatabaseManager *m_databaseManager;

    QString m_vtableName;
    QString m_tableName;
};

#endif // MAINWINDOW_H
