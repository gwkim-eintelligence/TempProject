#include "intrinsic_param.h"
#include "ui_intrinsic_param.h"
#include "intrinsic_param_calculate.cpp"


intrinsic_param::intrinsic_param(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::intrinsic_param)
{
    ui->setupUi(this);
    ui->progressBar_calProcess->setValue(0);

    QImage *Img = new QImage();
    QPixmap *buffer = new QPixmap();

    if(Img->load("./image/inCalImg.png"))
    {
        *buffer = QPixmap::fromImage(*Img);
        //*buffer = buffer->scaled(Img->width(),Img->height());
        *buffer = buffer->scaled(800, 259.710144928);
    }
    else
    {
        QMessageBox mbox;
        mbox.critical(this,"inCalImg Error"," inCalImg Path check !!!! ", mbox.Yes);
    }
    //QLabel *lbView = new QLabel(this);
    ui->Label_inCalImg->setPixmap(*buffer);
    ui->Label_inCalImg->resize(buffer->width(),buffer->height());
    ui->Label_inCalImg->move(50,320);
    ui->Label_inCalImg->show();


}


void intrinsic_param::closeEvent (QCloseEvent *event)
{
    QWidget::closeEvent(event);
    std::cout << "closeEvent " << std::endl;
}

intrinsic_param::~intrinsic_param()
{
    delete ui;
}

void intrinsic_param::on_pushButton_VideoLoad_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, "open File", "../", "File (*.mp4 *.avi)");
    ui->textBrowser_VideoPath->setText(file_path);
}

void intrinsic_param::on_pushButton_SavePath_clicked()
{
    QString folder_path = QFileDialog::getExistingDirectory(this, "folder select", "../", QFileDialog::ShowDirsOnly);
    // QDir::currentPath()
    ui->textBrowser_SavePath->setText(folder_path);
}

void intrinsic_param::on_textBrowser_VideoPath_textChanged()
{
    QString file_path = ui->textBrowser_VideoPath->toPlainText();
    if(file_path.length() > 1){
        m_startCheck[0]=1;
        m_path[0]=file_path;
    }
    else
        m_startCheck[0]=0;
}

void intrinsic_param::on_textBrowser_SavePath_textChanged()
{
    QString folder_path = ui->textBrowser_SavePath->toPlainText();
    if(folder_path.length() > 1){
        m_startCheck[1]=1;
        m_path[1]=folder_path;
        //std::cout<<folder_path<<std::endl;
        //std::cout<<folder_path.toStdString()<<std::endl;
    }
    else
        m_startCheck[1]=0;
}

void intrinsic_param::on_textEdit_ChessHorizontal_textChanged()
{
    QString chess_horizontal = ui->textEdit_ChessHorizontal->toPlainText();
    m_chess_pattern[0]= chess_horizontal.toInt();

    if(chess_horizontal.length() > 0 && m_chess_pattern[0] > 5 && m_chess_pattern[0] < 15){
        m_startCheck[2]=1;
        //m_chess_horizontal= chess_horizontal.split(" ")[0].toInt();
    }
    else
        m_startCheck[2]=0;
}

void intrinsic_param::on_textEdit_ChessVertical_textChanged()
{
    QString chess_vertical = ui->textEdit_ChessVertical->toPlainText();
    m_chess_pattern[1]= chess_vertical.toInt();

    if(chess_vertical.length() > 0 && m_chess_pattern[1] > 5  && m_chess_pattern[1] < 15){
        m_startCheck[3]=1;
    }
    else
        m_startCheck[3]=0;
}

void intrinsic_param::on_textEdit_FPS_textChanged()
{
    QString fps = ui->textEdit_FPS->toPlainText();
    int fps_temp= fps.toInt();

    if(fps.length() > 0 && fps_temp >= 1 && fps_temp < 10){
        m_fps = fps_temp;
        m_startCheck[4]=1;
    }
    else
        m_startCheck[4]=0;
}

void intrinsic_param::on_pushButton_Start_clicked()
{
    int video2MatProcess= 0;
    int interCameraCalibrationProcess= 0;
    int autoCalibrationProcess = 0;
    int principalPointErrorProcess = 0;
    int saveImgProcess = 0;
    int reprojectionProcess = 0;
    int undistortProcess= 0;
    int saveTxtProcess = 0;
    double totalAvgErr = 1.0;

    intrinsic_param_calculate inParamCal = intrinsic_param_calculate();
    ui->progressBar_calProcess->setValue(10);
    //ui->progressBar_calProcess->setAlignment(Qt.AlignCenter);

    if(m_startCheck[0] + m_startCheck[1] + m_startCheck[2] + m_startCheck[3] + m_startCheck[4] == 5){
        video2MatProcess= inParamCal.video2Mat(m_path[0].toStdString(), m_fps);
        ui->progressBar_calProcess->setValue(30);

        interCameraCalibrationProcess = inParamCal.interCameraCalibration(m_chess_pattern);
        ui->progressBar_calProcess->setValue(60);

        autoCalibrationProcess = inParamCal.autoCalibration(totalAvgErr);
        ui->textBrowser_ReprojectionError->setText(QString::number(totalAvgErr));
        ui->progressBar_calProcess->setValue(80);

        principalPointErrorProcess = inParamCal.principalPointError();
        if(principalPointErrorProcess == -1){
            std::cout<<"principalPointError"<<std::endl;
            ui->textBrowser_PrincipaPointError->setText("Error");
        }else{
            ui->textBrowser_PrincipaPointError->setText("PASS");
        }
        saveImgProcess = inParamCal.saveResultsImg(m_path[1].toStdString());
        reprojectionProcess = inParamCal.reprojectionPattern(m_path[1].toStdString());
        ui->progressBar_calProcess->setValue(90);

        saveTxtProcess = inParamCal.saveResultsTxt(m_path[1].toStdString());
        undistortProcess = inParamCal.undistortTest(m_path[1].toStdString());
        ui->progressBar_calProcess->setValue(100);
    }
    else{
        QMessageBox mbox;
        mbox.critical(this,"Input Error"," File Path, Chess Pattern, FPS check !!!! ", mbox.Yes);
    }
}

void intrinsic_param::on_pushButton_exit_clicked()
{
    this->close();
}

