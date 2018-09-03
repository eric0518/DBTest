#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cdatabasemanager.h"
#include "ctextdatareader.h"
#include <QDebug>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("DB Test"));
    setMinimumHeight(500);
    setMinimumWidth(1000);

    m_databaseManager = new CDatabaseManager(this);
    m_databaseManager->openDatabase("vposts.db");

    m_vtableName = "vposts";
    m_tableName = "posts";

    m_databaseManager->createTable(m_vtableName, QStringList() << "title" << "body", true);
    m_databaseManager->createTable(m_tableName, QStringList() << "title" << "body", false);

    connect(ui->ftsQueryButton, &QPushButton::clicked, [=](){
        ftsQueryButtonClicked();
    });

    connect(ui->queryButton, &QPushButton::clicked, [=]() {
        queryButtonClicked();
    });

    connect(ui->execButton, &QPushButton::clicked, [=]() {
         createTestData();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// private slots
void MainWindow::queryButtonClicked()
{
    QString text = ui->lineEdit->text().trimmed();

    QVariant countResult =  m_databaseManager->countQuery(m_tableName, false);
    int totalRowCount = countResult.toList().at(0).toMap().values().at(0).toInt();
    ui->totalCountLabel->setText(QString("%1").arg(totalRowCount));

    QTime time;
    time.start();
    QVariant result = m_databaseManager->searchQuery(m_tableName, text, false);
    int elapsed = time.elapsed();

    ui->resultCountLabel->setText(QString("%1").arg(result.toList().size()));
    ui->spentLabel->setText(QString("%1 ms").arg(elapsed));
}

void MainWindow::ftsQueryButtonClicked()
{
    QString text = ui->ftsLineEdit->text().trimmed();

    QVariant countResult =  m_databaseManager->countQuery(m_vtableName, true);
    int totalRowCount = countResult.toList().at(0).toMap().values().at(0).toInt();
    ui->ftsTotalCountLabel->setText(QString("%1").arg(totalRowCount));

    QTime time;
    time.start();
    QVariant result = m_databaseManager->searchQuery(m_vtableName, text, true);
    int elapsed = time.elapsed();

    ui->ftsResultCountLabel->setText(QString("%1").arg(result.toList().size()));
    ui->ftsSpentLabel->setText(QString("%1 ms").arg(elapsed));
}

void MainWindow::createTestData()
{
    bool isVirtual = ui->checkBox->isChecked();
    quint64 count = ui->countSpinBox->value();

    createTestData(count, isVirtual);
}

int MainWindow::createTestData(quint64 rowCount, bool isVirtual)
{
    CTextDataReader reader;
    QString dirName("C:/DBTest2/DataCollector/data/TextDataEN");
    if (!reader.openDirectory(QDir(dirName)))
    {
        qDebug() << "Failed to open file";
        return 0;
    }

    QString tableName = isVirtual ? m_vtableName : m_tableName;

    QStringList record;
    long index = 0;
    while ((record = reader.readRecordText(2)).size() && (index < rowCount))
    {
        if (!record.size())
        {
            continue;
        }

        if (m_databaseManager->insertRecord(tableName, record, 2))
        {
            ++ index;
        }

        if (index % 100 == 0)
        {
            qDebug() << "index = " << ++ index;
        }
    }

    qDebug() << "index = " << index;

    return index + 1;
}
