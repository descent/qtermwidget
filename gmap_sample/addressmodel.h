#include <QtCore>

class AddressModel : public QStandardItemModel
{

    public:
        AddressModel(QObject *parent=0) : QStandardItemModel(0,1,parent)
    {
        initModel();
    }

        ~AddressModel()
        {}

        void initModel()
        {
            foreach( QString address, this->addresses() ) {
                QModelIndex idx = this->index(this->rowCount(), 0 ,QModelIndex() );

                QStringList addressLines = address.split(",");

                QStandardItem *item = new QStandardItem;
                this->insertRow(this->rowCount(),item);

                item->setData(addressLines.first(), Qt::DisplayRole);
                addressLines.removeFirst();

                item->setData(addressLines.join(",").trimmed(),Qt::UserRole);
            }
        }

        QStringList addresses() {
            QFile file("addresses.txt");
            if( file.open(QFile::ReadOnly) )
                return QString(file.readAll()).split(QChar('\n'));
            return QStringList();
       }
};
