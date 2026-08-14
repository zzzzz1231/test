#include "opencvHelper.h"
#include <Ranger3.h>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QtGui/QImageReader>
#include <QtCore/QDir>
#include <QtGui/QStandardItemModel>
#include <QFileDialog>
#include <QInputDialog>

// todo: check status, reconnect camera

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // connect slots
    connect(ui->pushButton_setParam, SIGNAL(clicked()), this, SLOT(on_btnSetPatam_Clicked()));
    connect(this, SIGNAL(_log_emit(const QString)), this, SLOT(_log(const QString)));

    // welcome image
    _viewer_welcome();
    ui->pushButton_setParam->setDisabled(true);

    auto local_ip = SickCam::getLocalIP();
    std::stringstream local_ip_info;
    local_ip_info << "Computer information:\n";
    for(size_t i=0; i<local_ip.size(); ++i)
    {
        local_ip_info
            << "Group " << i << ":\n"
            << "    IP address = " << local_ip[i][0] << "\n"
			<< "    Submask    = " << local_ip[i][1] << "\n"
			<< "    IF name    = " << local_ip[i][2] << "\n--------------------\n\n";
    }
    _log(local_ip_info.str().c_str());
    m_save_image_path = "";


    // --- Init Ranger3 environment.
    m_Ranger3Shared = std::make_shared<SickCam::Ranger3Shared>(
            "",											/* 日志文件 */
            SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
            true,										/* 是否打印信息到屏幕 */
            false										/* 是否打印信息到文件 */
    );

    _scan();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionRange_triggered()
{
    ui->label_viewer->setText("Viewer(Range)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::RAN;
}

void MainWindow::on_actionReflectance_triggered()
{
    ui->label_viewer->setText("Viewer(Reflectance)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::REF;
}

void MainWindow::on_actionScatter_triggered()
{
    ui->label_viewer->setText("Viewer(Scatter8)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::SCA_8;
}

void MainWindow::on_action3D_triggered()
{
    ui->label_viewer->setText("Viewer(3D)");
}

void MainWindow::on_actionExport_Parameters_triggered()
{
    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        QString file_path = QFileDialog::getSaveFileName(this, "Please select a path to save parameter file.", "./");
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->saveParameterToCSV(file_path.toStdString());
        if(err == SickCam::CAM_STATUS::All_OK)
        {
            QMessageBox msg(QMessageBox::Information, "Information", "Export parameter file to " + file_path + " ok!", QMessageBox::Yes);
            _log("Ranger3Studio: Export parameter file to " + file_path + " ok!");
            msg.exec();
        }
        else
        {
            QMessageBox msg(QMessageBox::Warning, "Warning", "Export parameter file to " + file_path + " failed! ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str(), QMessageBox::Yes);
            _log("Ranger3Studio: Export parameter file to " + file_path + " failed! ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str());
            msg.exec();
        }
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Device status should be connected or stopped!", QMessageBox::Yes);
        _log("Ranger3Studio: [Warning] Device status should be connected or stopped!");
        msg.exec();
    }
}

void MainWindow::on_actionImport_Parameter_triggered()
{
    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        QString file_path = QFileDialog::getOpenFileName(this, "Please select a csv file.", "./");
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->loadParameterFrCSV(file_path.toStdString());
        if(err == SickCam::CAM_STATUS::All_OK)
        {
            QMessageBox msg(QMessageBox::Information, "Information", "Import parameter file from " + file_path + " ok!", QMessageBox::Yes);
            _log("Ranger3Studio: Import parameter file from " + file_path + " ok!");
            msg.exec();
        }
        else
        {
            QMessageBox msg(QMessageBox::Information, "Information", "Import parameter file from " + file_path + " failed!ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str(), QMessageBox::Yes);
            _log("Ranger3Studio: Import parameter file from " + file_path + " failed! ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str());
            msg.exec();
        }
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Device status should be connected or stopped!", QMessageBox::Yes);
        _log("Ranger3Studio: [Warning] Device status should be connected or stopped!");
        msg.exec();
    }
}

void MainWindow::on_actionScan_Device_triggered()
{
    _scan();
}

void MainWindow::on_actionOperation_triggered()
{
    if(m_R3_CTRL.empty())
    {
        _log("Ranger3Studio: No Device!");
        return;
    }

    if(m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_DISCONNECTED)
    {
        // --- Connect Ranger3
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->connectCamera(onCallback_LostHeartBeat, 1000, this);
        if (SickCam::CAM_STATUS::All_OK == err)
        {
            _log("Ranger3Studio: Connect " + m_currentSelection);
            _update_menu();

            //ui->pushButton_setParam->setDisabled(false);
            _update_parameter();
        }
        else
        {
            _log("Ranger3Studio: [Error] Connect " + m_currentSelection + " failed, ErrorCode: " +
                 SickCam::CAM_STATUS_str(m_R3_CTRL.at(m_currentSelection).m_this->getStatus()).c_str());
        }
    }
    else if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
             m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        // --- Disconnect Ranger3
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->disconnectCamera();
        if (SickCam::CAM_STATUS::All_OK == err)
        {
            _log("Ranger3Studio: Disconnect " + m_currentSelection);
            _update_menu();

            _clear_parameter_tree_view();
            //ui->pushButton_setParam->setDisabled(true);
            _viewer_welcome();
        }
        else
        {
            _log("Ranger3Studio: [Error] Disconnect " + m_currentSelection + " failed, ErrorCode: " +
                 SickCam::CAM_STATUS_str(m_R3_CTRL.at(m_currentSelection).m_this->getStatus()).c_str());
        }
    }
    else
    {
        _log("Ranger3Studio: [Error] Connect/Disconnect " + m_currentSelection + ", ErrorCode: " +
             SickCam::CAM_STATUS_str(m_R3_CTRL.at(m_currentSelection).m_this->getStatus()).c_str());
    }
}

void MainWindow::on_actionLoad_Image_triggered()
{
    QString file_path = QFileDialog::getOpenFileName(this, "Please select a dat|xml file.", "./");
    double Y_R = QInputDialog::getDouble(this, "Image Information", "Please input Y resolution: ", 1.0);
    SickCam::ImgT img;
    if(img.loadFromIconFile(file_path.toStdString(), Y_R))
    {
        for(auto dataName : img.getAvalibleDataNames())
        {
            auto src = InterfaceOpenCV::getMat(img, dataName);
            cv::resize(src, src, cv::Size(0, 0) ,0.25, 0.25);
            cv::imshow(SickCam::ImgT::DN2Str(dataName) +" : "+ file_path.toStdString(), src);
        }
    }
    else
    {
        _log("Ranger3Studio: [Error] Load image from " + file_path + " failed! \nY resolution is " + QString(std::to_string(Y_R).c_str()));
        QMessageBox msg(QMessageBox::Warning, "Warning", "Ranger3Studio: [Error] Load image from " + file_path + " failed! \nY resolution is " + QString(std::to_string(Y_R).c_str()), QMessageBox::Yes);
        msg.exec();
    }
}

void MainWindow::on_actionVersion_triggered()
{
    auto local_ip = SickCam::getLocalIP();
    std::stringstream local_ip_info;
    local_ip_info << "Computer information:\n";
    for(size_t i=0; i<local_ip.size(); ++i)
    {
        local_ip_info
			<< "Group " << i << ":\n"
			<< "    IP address = " << local_ip[i][0] << "\n"
			<< "    Submask    = " << local_ip[i][1] << "\n"
			<< "    IF name    = " << local_ip[i][2] << "\n--------------------\n\n";
    }

    auto version = SickCam::VER::__version__();
    version = local_ip_info.str() + version;
    QMessageBox::about(NULL, "Version", QString(version.c_str()));
}

void MainWindow::_log(const QString & log)
{
    auto previousLog = ui->textEdit_Log->toPlainText();
    auto time_str = SickCam::CustomerLog::time();
    ui->textEdit_Log->setPlainText(previousLog + "\n\n" + time_str.c_str() +  "\n" + log);
    ui->textEdit_Log->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void MainWindow::_scan() {
    // clear devices
    for(auto sub : m_R3_CTRL)
    {
        if(sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
        {
            sub.second.m_this->stopCamera();
            while (sub.second.m_this->getStatus() != SickCam::CAM_STATUS::CAM_IS_STOPPED) {
                __sleep1MS(50);
            }
        }

        if( sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
            sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED  )
        {
            sub.second.m_this->disconnectCamera();
            while (sub.second.m_this->getStatus() != SickCam::CAM_STATUS::CAM_IS_DISCONNECTED) {
                __sleep1MS(50);
            }
        }
        sub.second.m_this->disconnectCamera();
    }
    m_R3_CTRL.clear();
    m_R3_CTRL = std::map<QString, RAN_CTRL>();

    // scan
    auto err = m_Ranger3Shared->scanDevice();
    if (SickCam::CAM_STATUS::All_OK == err)
    {
        std::stringstream ss;
        ss << "\nFound\n";
        auto devicesName = m_Ranger3Shared->getConDevList();
        auto ItemModel = new QStandardItemModel(this);
        int deviceId = 0;
        for (auto sub : devicesName)
        {
            ss << "  * " << sub.first;
            auto ip = sub.second->getIp();
            auto submask = sub.second->getSubnet();
            auto mac = sub.second->getMac();
            ss << ", \n     ip=" << ip
               << ", \n     submask=" << submask
               << ", \n     mac=" << mac;

            auto pCam = std::make_shared<SickCam::Ranger3>(m_Ranger3Shared, ip, true);

            err = pCam->isReady();
            if (err == Ecode::All_OK)
            {
                QStandardItem *item = new QStandardItem(QString(sub.first.c_str()));
                item->setEditable(false);
                ItemModel->appendRow(item);

                ss << ", \n     init ok!" << std::endl;
                if (m_currentSelection.isEmpty())
                    m_currentSelection = QString(sub.first.c_str());

                m_R3_CTRL.insert({
                                     QString(sub.first.c_str()),
                                     RAN_CTRL(QString(sub.first.c_str()),
                                              SickCam::DataNames::SEN, //deviceId++,
                                              QString(ip.c_str()),
                                              QString(submask.c_str()),
                                              QString(mac.c_str()),
                                              pCam,
                                              item)
                                 });
            } else
                ss << ", \n    init failed! ErrorCode: " << SickCam::CAM_STATUS_str(err).c_str() << std::endl;
        }

        ui->listView_Device->setModel(ItemModel);  //
        _log(QString("Ranger3Studio:") + ss.str().c_str());
    }
    else
    {
        auto ItemModel = new QStandardItemModel(this);
        ui->listView_Device->setModel(ItemModel);  //
        m_currentSelection = "";

        ui->pushButton_setParam->setDisabled(true);
        _clear_parameter_tree_view();
        _log(QString("Ranger3Studio: Scan done! No device found! ErrorCode: ") + SickCam::CAM_STATUS_str(err).c_str());
    }
}

void MainWindow::_update_parameter()
{
    SickCam::Categories cate;
    SickCam::Params para;

    if(m_R3_CTRL.count(m_currentSelection) == 1) {
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->getAllParameterInfo(cate, para, true);
        if (err != SickCam::CAM_STATUS::All_OK) {
            _log(QString("Ranger3Studio:[Error] Get Parameters from device failed. ErrorCode: ") + SickCam::CAM_STATUS_str(err).c_str());
            _clear_parameter_tree_view();
            return;
        }
    }

    QStandardItemModel* model = new QStandardItemModel(ui->treeView_Parameters);
    model->setHorizontalHeaderLabels(QStringList({"Parameter", "Value", "Optional Values" }));

    for(auto category : cate)
    {
        // category
        QList<QStandardItem*> itemList;
        QStandardItem* itemCate = new QStandardItem(category.c_str());
        itemList.append(itemCate);

        // parameters in this category
        for(auto sub : para[category])
        {
            QList<QStandardItem*> itemList2;
            QStandardItem* item2_left = new QStandardItem(sub->getCsvName().c_str());
            item2_left->setEditable(false);

            QStandardItem* item2_middle = new QStandardItem(sub->getStrValue().c_str());

            QString qstr_opts;
            auto opts = sub->getOptions();
            for(auto op : opts)
            {
                qstr_opts += op.c_str();
                qstr_opts += "  ";
            }

            QStandardItem* item2_right = new QStandardItem(qstr_opts);

            itemList2.append(item2_left);
            itemList2.append(item2_middle);
            itemList2.append(item2_right);
            itemCate->appendRow(itemList2);
        }
        model->appendRow(itemList);
    }
    ui->treeView_Parameters->setModel(model);
    ui->treeView_Parameters->expandAll();
    //ui->pushButton_setParam->setDisabled(false);
}

void MainWindow::_update_selectionLabel()
{
    ui->label->setText(
            QString("Device List : (") +
            m_R3_CTRL.at(m_currentSelection).m_mac + " => " +
            SickCam::CAM_STATUS_str(m_R3_CTRL.at(m_currentSelection).m_this->getStatus()).c_str() + ")"
    );
}

void MainWindow::_update_device_list()
{
    auto ItemModel = new QStandardItemModel(this);
    int deviceId = 0;

    for (auto sub : m_R3_CTRL) {
        QStandardItem *item = sub.second.m_gui_DevItem;
        item->setEditable(false);
        ItemModel->appendRow(item);
    }

    if (m_R3_CTRL.count(m_currentSelection) != 1)
        m_currentSelection = m_R3_CTRL.empty() ? "" : m_R3_CTRL.begin()->first;

    ui->listView_Device->setModel(ItemModel);
    emit _log_emit("Ranger3Studio: update_device_list");
}

void MainWindow::_update_menu()
{
    if(m_R3_CTRL.count(m_currentSelection) == 1)
    {
        auto status = m_R3_CTRL.at(m_currentSelection).m_this->getStatus();
        switch (status) {
            case SickCam::CAM_STATUS::CAM_IS_CONNECTED:
                ui->actionOperation->setText("Disconnect");
                ui->pushButton_setParam->setDisabled(false);
                break;
            case SickCam::CAM_STATUS::CAM_IS_DISCONNECTED:
                ui->actionOperation->setText("Connect");
                ui->pushButton_setParam->setDisabled(true);
                break;
            case SickCam::CAM_STATUS::CAM_IS_STARTED:
                ui->actionStart2->setText("Stop");
                ui->pushButton_setParam->setDisabled(true);
                break;
            case SickCam::CAM_STATUS::CAM_IS_STOPPED:
                ui->actionStart2->setText("Start");
                ui->pushButton_setParam->setDisabled(false);
                break;
            default:
                ui->actionOperation->setText("Connect");
                ui->actionStart2->setText("Start");
                ui->pushButton_setParam->setDisabled(true);
        }
        _update_selectionLabel();
    }
}

void MainWindow::_clear_parameter_tree_view()
{
    QStandardItemModel* model = new QStandardItemModel(ui->treeView_Parameters);
    model->setHorizontalHeaderLabels(QStringList({"Parameter", "Value"}));
    ui->treeView_Parameters->setModel(model);
    ui->pushButton_setParam->setDisabled(true);
}

void MainWindow::_viewer_welcome()
{
    try {
        cv::Mat src;
#ifdef __linux__
        src = cv::imread("/usr/local/lib/SICK.jpeg");
#endif
#ifdef _WIN32
		std::string path;
		auto p = getenv("SICK_GENICAM_SDK");// Load from environment path 
		if (p != nullptr)
			path = std::string(p);

		src = cv::imread(path + "/SICK/help/SICK.jpeg");
#endif
		ui->label_Image->setPixmap(QPixmap::fromImage(cvHelper::mat2qimRGB(src))); // show cv::Mat
    }
    catch(...)
    {
        _log("Ranger3Studio: Welcome image(/usr/local/lib/SICK.jpeg) no found! ");
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    for(auto sub : m_R3_CTRL)
    {
        if(sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
        {
            sub.second.m_this->stopCamera();
            while (sub.second.m_this->getStatus() != SickCam::CAM_STATUS::CAM_IS_STOPPED)
                __sleep1MS(50);
        }

        if( sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
            sub.second.m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED  )
        {
            sub.second.m_this->disconnectCamera();
            while (sub.second.m_this->getStatus() != SickCam::CAM_STATUS::CAM_IS_DISCONNECTED) {
                __sleep1MS(50);
            }
        }
        sub.second.m_this->disconnectCamera();
    }
    m_R3_CTRL.clear();
    _log("Ranger3Studio: Exit.");
}

void MainWindow::on_listView_Device_clicked(const QModelIndex &index)
{
    QString strTemp;
    m_currentSelection = index.data().toString();
    _update_selectionLabel();

    if( m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED  )
    {
        auto ip = m_R3_CTRL.at(m_currentSelection).m_this->getIp();
        auto mac = m_R3_CTRL.at(m_currentSelection).m_this->getMac();
        _update_parameter();
        _log("Ranger3Studio: Select " + m_currentSelection + "\n  ip =" + ip.c_str() + "\n  mac =" + mac.c_str());
    }
    else
    {
        _clear_parameter_tree_view();
        _log("Ranger3Studio: Select " + m_currentSelection + "\n  ip =" + m_R3_CTRL.at(m_currentSelection).m_ip + "\n  mac =" + m_R3_CTRL.at(m_currentSelection).m_mac);
    }
}

void MainWindow::on_listView_Device_doubleClicked(const QModelIndex &index)
{
}

void MainWindow::on_actionStart2_triggered()
{
    if(m_R3_CTRL.count(m_currentSelection) != 1)    {
        _log("Ranger3Studio: No such Device:" + m_currentSelection + "!");
        return;
    }

    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)    {
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->stopCamera();
        if (SickCam::CAM_STATUS::All_OK == err)
        {
            _log("Ranger3Studio: Stop " + m_currentSelection);
            _update_menu();
            _update_parameter();
        }
    }
    else if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
            m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->startCamera(&onCallback_GetImage, this);
        if (SickCam::CAM_STATUS::All_OK == err) {
            _log("Ranger3Studio: Start " + m_currentSelection);
            _update_menu();
        }
    }
    else
        _log("Ranger3Studio: [Error] " + m_currentSelection + ", ErrorCode: " + SickCam::CAM_STATUS_str(m_R3_CTRL.at(m_currentSelection).m_this->getStatus()).c_str());
}

void SICK_CALLBACK
onCallback_GetImage		(SickCam::ImgT * pImgD, void * any)
{
    MainWindow* pThis = (MainWindow*)any;

    if(pThis->m_R3_CTRL.count(pThis->m_currentSelection) != 1)
        return;

    auto show_mode = pThis->m_R3_CTRL.at(pThis->m_currentSelection).m_show_mode;
    pThis->ui->label_viewer->setText(QString("Viewer：") +
                              (pImgD->has(show_mode)
                               ?(  SickCam::ImgT::DN2Str(show_mode) + ", current id=" + std::to_string(pImgD->get_ID()) ).c_str()
                               :(  SickCam::ImgT::DN2Str(show_mode) + " no available! Please select a suitable Viewer"  ).c_str()
                              ));

    cv::Mat img;
    switch (show_mode)
    {
        case SickCam::DataNames::RAN : // Range
            img = InterfaceOpenCV::uint16_tPtrToMat(
                    pImgD->has(DN::RAN) ? (uint16_t *) pImgD->getData(DN::RAN) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            img = InterfaceOpenCV::remapToUint8_t(img);
            if(!pThis->m_save_image_path.isEmpty())
            {
                if(pImgD->SaveToIconFile(
                        pThis->m_save_image_path.toStdString()+std::to_string(pImgD->get_ID()),
                        {SickCam::DataNames::RAN, SickCam::DataNames::REF, SickCam::DataNames::SCA_8, SickCam::DataNames::MAR},
                        false))
                        emit pThis->_log_emit("Ranger3Studio: Save raw image to " + pThis->m_save_image_path + std::to_string(pImgD->get_ID()).c_str() + " ok!");
                else
                        emit pThis->_log_emit("Ranger3Studio: [Error] Save raw image to " + pThis->m_save_image_path + std::to_string(pImgD->get_ID()).c_str() + ".dat|xml failed!");
            }
            break;
        case SickCam::DataNames::REF : // Reflectance
            img = InterfaceOpenCV::uint8_tPtrToMat(
                    pImgD->has(DN::REF) ? (uint8_t *) pImgD->getData(DN::REF) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            break;
        case SickCam::DataNames::RAN_CAL :
            img = InterfaceOpenCV::floatPtrToMat(
                    pImgD->has(DN::RAN_CAL) ? (float *) pImgD->getData(DN::RAN_CAL) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            if(!pThis->m_save_image_path.isEmpty())
            {
                if(pImgD->SaveToIconFile(
                        pThis->m_save_image_path.toStdString() + "_CAL_" + std::to_string(pImgD->get_ID()),
                        {SickCam::DataNames::RAN_CAL, SickCam::DataNames::REF_CAL, SickCam::DataNames::SCA_8_CAL, SickCam::DataNames::MAR},
                        true))
                        emit pThis->_log_emit("Ranger3Studio: Save calibrated image to " + pThis->m_save_image_path + "_CAL_" + std::to_string(pImgD->get_ID()).c_str() + " ok!");
                else
                        emit pThis->_log_emit("Ranger3Studio: [Error] Save calibrated image to " + pThis->m_save_image_path + "_CAL_" + std::to_string(pImgD->get_ID()).c_str() + ".dat|xml failed!");
            }
            break;
        case SickCam::DataNames::REF_CAL :
            img = InterfaceOpenCV::uint8_tPtrToMat(
                    pImgD->has(DN::REF_CAL) ? (uint8_t *) pImgD->getData(DN::REF_CAL) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            break;
        case SickCam::DataNames::SCA_8 :
            img = InterfaceOpenCV::uint8_tPtrToMat(
                    pImgD->has(DN::SCA_8) ? (uint8_t *) pImgD->getData(DN::SCA_8) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            break;
        case SickCam::DataNames::SEN : // sensor
            img = InterfaceOpenCV::uint8_tPtrToMat(
                    pImgD->has(DN::SEN) ? (uint8_t *) pImgD->getData(DN::SEN) : nullptr,
                    pImgD->get_rows(),
                    pImgD->get_cols()
            );
            if(!pThis->m_save_image_path.isEmpty())
            {
                if (pImgD->SaveToIconFile(
                        pThis->m_save_image_path.toStdString() + std::to_string(pImgD->get_ID()),
                        {SickCam::DataNames::SEN},
                        false))
                        emit pThis->_log_emit("Ranger3Studio: Save sensor image to " + pThis->m_save_image_path +
                                       std::to_string(pImgD->get_ID()).c_str() + " ok!");
                else
                        emit pThis->_log_emit("Ranger3Studio: [Error] Save sensor image to " + pThis->m_save_image_path +
                                       std::to_string(pImgD->get_ID()).c_str() + ".dat|xml failed!");
            }
            break;
        default:
            emit pThis->_log_emit(QString("[debug] def Viewer：") + SickCam::ImgT::DN2Str(show_mode).c_str());
            break;
    }

    if (!img.empty())
    {
        auto size = pThis->ui->label_Image->size();
        auto h = size.height();
        auto w = size.width();

        auto maxL = h < w ? w : h;
        auto max_is_w = h < w;

        double factor(0);

        if(max_is_w)
            factor = (double)w/img.cols;
        else
            factor = (double)h/img.rows;

        cv::resize(img, img, cv::Size(), factor, factor);
        img.convertTo(img, CV_8UC1);
        pThis->ui->label_Image->setPixmap(QPixmap::fromImage(cvHelper::mat2qimGray(img))); // show cv::Mat
    }
    else
    {
        pThis->_viewer_welcome();
    }
}

void SICK_CALLBACK
onCallback_LostHeartBeat(std::string* name, std::string* ip, std::string* mac, std::string* msg, void* any)
{
    MainWindow* pThis = (MainWindow*)any;

    QString qname = QString(name->c_str());
    emit pThis->_log_emit(QString(msg->c_str()));

    auto pCam = pThis->m_R3_CTRL.at(qname).m_this;
    pThis->_update_selectionLabel();
    pThis->_update_menu();

    pThis->m_R3_CTRL.erase(qname);

    pThis->_update_device_list();
    pThis->_update_parameter();

    while (true)
    {
        emit pThis->_log_emit(QString("Try reconnect : ") + qname);
        auto ec = pCam->reconnectCamera();
        emit pThis->_log_emit(QString("Try reconnect : return ") + SickCam::CAM_STATUS_str(ec).c_str());
        if(SickCam::CAM_STATUS::All_OK == ec)
        {
            QStandardItem *item = new QStandardItem(qname);
            pThis->m_R3_CTRL.insert({ qname,
                                      RAN_CTRL(qname,
                                          SickCam::DataNames::SEN,
                                          QString(pCam->getIp().c_str()),
                                          QString(pCam->getSubNet().c_str()),
                                          QString(pCam->getMac().c_str()),
                                          pCam,
                                          item)
                             });

            pThis->_update_device_list();
            pThis->_update_menu();
            break;
        }

        emit pThis->_log_emit(QString("Try reconnect : ") + pCam->getMac().c_str() + " after 2000 ms.");
        __sleep1MS(2000);
    }
}

void MainWindow::on_actionLoad_Calibration_File_triggered()
{
    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        QString file_path = QFileDialog::getOpenFileName(this, "Please select a json/xml file.", "./");
        auto err = m_R3_CTRL.at(m_currentSelection).m_this->setCalibraPath(file_path.toStdString());
        if(err == SickCam::CAM_STATUS::All_OK)
        {
            QMessageBox msg(QMessageBox::Information, "Information", "Load calibration file from " + file_path + " ok!", QMessageBox::Yes);
            _log("Ranger3Studio: Load calibration file from " + file_path + " ok!");
            msg.exec();
        }
        else
        {
            QMessageBox msg(QMessageBox::Information, "Information", "Load calibration file from " + file_path + " failed!ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str(), QMessageBox::Yes);
            _log("Ranger3Studio: Load calibration file from " + file_path + " failed! ErrorCode=" + SickCam::CAM_STATUS_str(err).c_str());
            msg.exec();
        }
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Device status should be connected or stopped!", QMessageBox::Yes);
        _log("Ranger3Studio: [Warning] Device status should be connected or stopped!");
        msg.exec();
    }
}

void MainWindow::on_actionSave_Image_triggered()
{
    if(!m_save_image_path.isEmpty())
    {
        m_save_image_path = "";
        ui->actionSave_Image->setText("Save Image");
        _log("Ranger3Studio: Stop saving image. " + m_save_image_path);
        return;
    }

    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        m_save_image_path = QFileDialog::getSaveFileName(this, "Please select a path_name to save Image.", "./");
        _log("Ranger3Studio: The image will be saved in: " + m_save_image_path);
        ui->actionSave_Image->setText("Stop Save Image");
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Device status should be connected or stopped!", QMessageBox::Yes);
        _log("Ranger3Studio: [Warning] Device status should be connected or stopped!");
        msg.exec();
    }
}

void MainWindow::on_actionCalibrate_image_triggered()
{

}

void MainWindow::on_actionSensor_triggered()
{
    ui->label_viewer->setText("Viewer(Sensor)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::SEN;
}

void MainWindow::on_actionChunkData_Mark_triggered()
{
    ui->label_viewer->setText("Viewer(ChunkData)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::MAR;
}

void MainWindow::on_actionCalibrated_Range_triggered()
{
    ui->label_viewer->setText("Viewer(Calibrated Range)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::RAN_CAL;
}

void MainWindow::on_actionCalibrated_Reflectance_triggered()
{
    ui->label_viewer->setText("Viewer(Calibrated Reflectance)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::REF_CAL;
}

void MainWindow::on_actionCalibrated_Scatter8_triggered()
{
    ui->label_viewer->setText("Viewer(Calibrated Scatter8)");
    m_R3_CTRL.at(m_currentSelection).m_show_mode = SickCam::DataNames::SCA_8_CAL;
}

void MainWindow::on_btnSetPatam_Clicked()
{
    bool ok=true;
    if (m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED ||
        m_R3_CTRL.at(m_currentSelection).m_this->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED)
    {
        QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->treeView_Parameters->model());
        for(int id=0; id < model->rowCount(); ++id)
        {
            if(model->item(id, 0)->hasChildren())
            {
                auto item = model->item(id, 0);
                for (int i=0; i < item->rowCount(); ++i)
                {
                    auto var_name  = item->child(i,0)->text();
                    auto var_value = item->child(i,1)->text();
                    auto err = m_R3_CTRL.at(m_currentSelection).m_this->setParameterValue(var_name.toStdString(),var_value.toStdString());
                    if(err != SickCam::CAM_STATUS::All_OK) {
                        ok = false;
                        _log("Ranger3Studio: [Error] Set " + var_name + " = " + var_value + ", Failed! ErrorCode = " +
                             SickCam::CAM_STATUS_str(err).c_str());
                    }
                }
            }
        }
    }

    if(ok)
        _log("Ranger3Studio: Set parameters ok! ");
}