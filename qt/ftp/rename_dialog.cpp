#include "rename_dialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

RenameDialog::RenameDialog( QWidget * parent , Qt::WindowFlags f ):QDialog(parent, f)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  ok_ = new QPushButton(tr("&ok"));
  cancel_ = new QPushButton(tr("&cancel"));
  rename_ = new QLineEdit();
  mainLayout->addWidget(rename_);
  mainLayout->addLayout(buttonLayout);
  buttonLayout->addWidget(ok_);
  buttonLayout->addWidget(cancel_);
  setLayout(mainLayout);
  connect(ok_, SIGNAL(clicked() ), this, SLOT(accept() ) );
  //connect(ok_, SIGNAL(clicked() ), this, SLOT(s_ok() ) );
  connect(cancel_, SIGNAL(clicked() ), this, SLOT(reject() ) );
  //rename_->hide();
}

int RenameDialog::get_filename(QString &path)
{
  path=rename_->text();
  return 0;
}

void RenameDialog::set_filename(const QString &path)
{
  rename_->setText(path);
}

void RenameDialog::set_ok_dialog()
{
  rename_->hide();
}
void RenameDialog::set_normal_dialog()
{
  rename_->show();
}
