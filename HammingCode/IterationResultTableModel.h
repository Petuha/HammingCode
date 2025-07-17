#pragma once

#include <QAbstractTableModel>
#include <QVariant>
#include <vector>
#include "../CoderDecoder/IterationResult.h"

class IterationResultTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    const size_t BATCH_SIZE = 2000;

    explicit IterationResultTableModel(std::vector<IterationResult>& results, bool isModifiedCode, QObject* parent = nullptr) :
        QAbstractTableModel(parent),
        results(results),
        isModifiedCode(isModifiedCode),
        fetchedCount(qMin(BATCH_SIZE, results.size())) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

private:
    std::vector<IterationResult>& results;
    bool isModifiedCode;
    size_t fetchedCount;
};