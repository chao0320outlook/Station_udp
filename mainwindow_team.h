#ifndef MAINWINDOW_TEAM_H
#define MAINWINDOW_TEAM_H

#include <QMainWindow>

#include <QLabel>
#include <QString>
#include <QVector>
#include <QByteArray>

#include <QtSql>
#include <QDataWidgetMapper>
#include "qwcomboboxdelegate.h"
#include "soccer_player.h"

namespace Ui
{
class MainWindow_team;
}

class MainWindow_team : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_team(QWidget *parent = 0);
    ~MainWindow_team();

private:

    QVector< Soccer_player > team_1;

    QVector< Soccer_player > team_2;

    Ui::MainWindow_team *ui;

    QSqlDatabase  DB;//数据库连接

    QSqlTableModel  *tabModel;  //数据模型

    QItemSelectionModel *theSelection; //选择模型

    QDataWidgetMapper   *dataMapper; //数据映射

    QWComboBoxDelegate   delegateSex; //自定义数据代理，性别
    QWComboBoxDelegate   delegateDepart; //自定义数据代理，部门

    bool data_base_open=false;

    void    openTable();//打开数据表
    void    getFieldNames();//获取字段名称,填充“排序字段”的comboBox
    void    updata();

signals:
     void player_massage(QVector<Soccer_player>,QVector<Soccer_player>);

private slots:

    //    void on_primeInsert(int row, QSqlRecord &record);//插入记录时，用于设置缺省字段数据
    void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);

    // QTableView的SelectionModel的行发生了变化，进行处理
    void on_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_actRecAppend_triggered();

    void on_actRecInsert_triggered();

    void on_actSubmit_triggered();

    void on_actRevert_triggered();

    void on_actRecDelete_triggered();

    void on_actPhoto_triggered();

    void on_actPhotoClear_triggered();

    void on_radioBtnAscend_clicked();

    void on_radioBtnDescend_clicked();

    void on_comboFields_currentIndexChanged(int index);

    void on_actOpenDB_triggered();

    void on_comboBox_currentIndexChanged(int index);


    void on_pushButton_clicked();
};

#endif // MAINWINDOW_TEAM_H
