#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "getSyntaxTree.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, NULL, NULL, "*.TNY *.tny");

    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->codeText->clear();
        QTextStream textStream(&file);
        while(!textStream.atEnd())
        {
            ui->codeText->append(textStream.readLine());
        }
    }
    else
    {
        QMessageBox ::information(NULL, NULL, "open file error");
    }
}

//生成语法树
void MainWindow::on_getSyntaxTreeBtn_clicked()
{
    ui->syntaxTreeText->clear();
    /* 将源码写入SAMPLE.txt中 */
    QFile sourceFile("SAMPLE.txt");
    sourceFile.open(QFile::WriteOnly);
    QTextStream out(&sourceFile);
    out<<ui->codeText->toPlainText();
    sourceFile.close();

    ui->syntaxTreeText->clear();
    //得到语法树
    getSyntaxTree();

    /* 将syntaxTree.txt 的内容显示 */
    QFile file("syntaxTree.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream read(&file);
        while(!read.atEnd())
        {
            ui->syntaxTreeText->append(read.readLine());
        }
    }
    else
    {
        QMessageBox ::information(NULL, NULL, "open file error");
    }
}

void MainWindow::on_saveCodeBtn_clicked()
{
    QString code = ui->codeText->toPlainText();

    QString fileName = QFileDialog::getSaveFileName(this, tr("保存tiny源码"),
                                                    "",
                                                    tr("tiny文件(*.TNY)"));
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this, "critical", tr("文件保存失败！"),
                              QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QTextStream stream(&file);

        stream << code;
        stream.flush();

        file.close();
    }
}

void MainWindow::on_saveSyntaxTreeBtn_clicked()
{
    QString syntaxTree = ui->syntaxTreeText->toPlainText();

    QString fileName = QFileDialog::getSaveFileName(this, tr("保存语法树"),
                                                    "",
                                                    tr("语法树文件(*.txt)"));
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this, "critical", tr("文件保存失败！"),
                              QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QTextStream stream(&file);

        stream << syntaxTree;
        stream.flush();

        file.close();
    }
}
