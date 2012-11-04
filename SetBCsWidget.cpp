#include "SetBCsWidget.h"
#include "ui_SetBCsWidget.h"
#include <QStringList>
#include <QTreeWidget>
#include "SetBCsItem.h"
#include "HexBC.h"

#include <vtkCollection.h>
#include <vtkSmartPointer.h>
#include <vtkIdList.h>

#include <QTreeWidgetItem>
#include <QList>

SetBCsWidget::SetBCsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetBCsWidget)
{
    ui->setupUi(this);
    QStringList headers;
    headers << tr("Name") << tr("Type");
    ui->treeWidget->setHeaderLabels(headers);

    allPatches = vtkSmartPointer<vtkCollection>::New();
    connect(ui->pushButtonNew,SIGNAL(clicked()),this,SLOT(slotCreateBC()));
    connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(slotBCchanged(QTreeWidgetItem *,int)));
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(slotShowPatchesOnSelection(QTreeWidgetItem*,int)));
    connect(ui->pushButtonSelect,SIGNAL(clicked()),this,SLOT(slotSelectPatches()));
    connect(ui->pushButtonDone,SIGNAL(clicked()),this,SIGNAL(done()));
}

SetBCsWidget::~SetBCsWidget()
{
    delete ui;
}

void SetBCsWidget::slotCreateBC()
{
    SetBCsItem *bc = new SetBCsItem(ui->treeWidget);
    bc->hexBC = vtkSmartPointer<HexBC>::New();

    bc->setFlags(bc->flags() | Qt::ItemIsEditable);
    bc->setText(0,tr("name"));
    bc->setText(1,tr("patch"));
    bc->hexBC->allPatches = allPatches;
    hexBCs->AddItem(bc->hexBC);


}

void SetBCsWidget::slotBCchanged(QTreeWidgetItem *item, int col)
{
    SetBCsItem *bc = static_cast<SetBCsItem*>(item);
    bc->hexBC->name=bc->text(0).toStdString();
    bc->hexBC->type=bc->text(1).toStdString();
}

void SetBCsWidget::slotSelectPatches()
{
    //If non selected
    QList<QTreeWidgetItem*> selectedTreeItem = ui->treeWidget->selectedItems();
    if(selectedTreeItem.count()>0)
    {
        SetBCsItem *bcItem = static_cast<SetBCsItem*>(selectedTreeItem[0]);
        vtkIdType hexBCId = hexBCs->IsItemPresent(bcItem->hexBC)-1;

        emit startSelectPatches(hexBCId);
    }
    else
    {
        std::cout << "select a boundary condition in the list first!" <<std::endl;
    }
}

void SetBCsWidget::slotSelectionDone(vtkIdList *selectedPatches)
{
    QList<QTreeWidgetItem*> selectedTreeItem = ui->treeWidget->selectedItems();
    SetBCsItem *bcItem = static_cast<SetBCsItem*>(selectedTreeItem[0]);
    vtkIdType hexBCId = hexBCs->IsItemPresent(bcItem->hexBC)-1;
    HexBC *hexBC = HexBC::SafeDownCast(hexBCs->GetItemAsObject(hexBCId));
    hexBC->patchIds->DeepCopy(selectedPatches);
    std::cout <<" patchIds: " << hexBC->patchIds->GetNumberOfIds()<<std::endl;

    emit resetInteractor();
}

void SetBCsWidget::slotShowPatchesOnSelection(QTreeWidgetItem *item, int col)
{
    //Reset all colors first!
    emit resetInteractor();
    SetBCsItem *bcitem = static_cast<SetBCsItem*>(item);
    bcitem->hexBC->setPatchColors(0.0,1.0,0.0);
    emit render();

}