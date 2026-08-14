#pragma once

#include <QMainWindow>
#include <Ranger3.h>
#include <QtWidgets/QTextEdit>
#include <QtGui/QStandardItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct RAN_CTRL
{
    RAN_CTRL(){};
    RAN_CTRL(
            const QString &_name,
            const SickCam::DataNames & _show_mode,
            const QString &_ip,
            const QString &_submask,
            const QString &_mac,
            std::shared_ptr<SickCam::Ranger3> _this,
            QStandardItem * _gui_DevItem

    )
    :m_name(_name)
    ,m_show_mode(_show_mode)
    ,m_ip(_ip)
    ,m_submask(_submask)
    ,m_mac(_mac)
    ,m_this(_this)
    ,m_gui_DevItem(_gui_DevItem)
    {
    }

    QString m_name;
    SickCam::DataNames m_show_mode; // 0:Range ;1:Reflectance 2:Scatter; 3:3D, 4:sensor; // 5 6 7
    QString m_ip;
    QString m_submask;
    QString m_mac;
    std::shared_ptr<SickCam::Ranger3> m_this;
    QStandardItem * m_gui_DevItem;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionRange_triggered();

    void on_actionReflectance_triggered();

    void on_actionScatter_triggered();

    void on_action3D_triggered();

    void on_actionExport_Parameters_triggered();

    void on_actionImport_Parameter_triggered();

    void on_actionScan_Device_triggered();

    void on_actionOperation_triggered();

    void on_actionLoad_Image_triggered();

    void on_actionVersion_triggered();

    void on_listView_Device_clicked(const QModelIndex &index);

    void on_listView_Device_doubleClicked(const QModelIndex &index);

    void on_actionStart2_triggered();

    void on_actionLoad_Calibration_File_triggered();

    void on_actionSave_Image_triggered();

    void on_actionCalibrate_image_triggered();

    void on_actionSensor_triggered();

    void on_actionChunkData_Mark_triggered();

    void on_actionCalibrated_Range_triggered();

    void on_actionCalibrated_Reflectance_triggered();

    void on_actionCalibrated_Scatter8_triggered();

    void on_btnSetPatam_Clicked();

    void _log(const QString & log);

signals:
    void _log_emit(const QString & log);

protected:

    void _scan();
    void _update_parameter();
    void _update_selectionLabel();
    void _update_device_list();
    void _update_menu();

    void _clear_parameter_tree_view();
    void _viewer_welcome();

    void closeEvent(QCloseEvent* event) override; // Qt window close event

private:
    Ui::MainWindow *ui;

    std::shared_ptr<SickCam::Ranger3Shared>  m_Ranger3Shared;

    std::map<QString, RAN_CTRL>     m_R3_CTRL;  // name, control parameters in this gui demo

    QString                     m_currentSelection; // name
    QString                     m_save_image_path;


#pragma region ON_CALL_FUNCTION
    friend
    void SICK_CALLBACK
    onCallback_GetImage		(SickCam::ImgT * pImgD, void * any);

    friend
    void SICK_CALLBACK
    onCallback_LostHeartBeat (std::string* name, std::string* ip, std::string* mac, std::string* msg, void* any);
#pragma endregion

};

void SICK_CALLBACK
onCallback_GetImage		(SickCam::ImgT * pImgD, void * any);

void SICK_CALLBACK
onCallback_LostHeartBeat (std::string* name, std::string* ip, std::string* mac, std::string* msg, void* any);
