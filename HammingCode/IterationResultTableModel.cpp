#include "IterationResultTableModel.h"

int IterationResultTableModel::rowCount(const QModelIndex& parent) const {
	return fetchedCount;
}

int IterationResultTableModel::columnCount(const QModelIndex& parent) const {
	return 8 + isModifiedCode;
}

QVariant IterationResultTableModel::data(const QModelIndex& index, int role) const {
	if (role == Qt::DisplayRole && index.isValid())
	{
		auto& item = results[index.row()];
		switch (index.column()) {
		case 0:	return QString::fromStdString(item.receivedBits);
		case 1:	return QString::fromStdString(item.restoredBits);
		case 2:	return QString::fromStdString(item.decodedBits);
		case 3:	return item.brokenCount;
		case 4:	return item.correctlyRestoredInEncodedCount;
		case 5:	return item.correctlyRestoredInDecodedCount;
		case 6:	return item.errorsInDecodedCount;
		case 7:	return QString::number(item.errorsInDecodedRatio, 'f', 2); ;
		case 8:	return item.correctModifiedVerdictCount;
		}
	}
	return QVariant();
}

QVariant IterationResultTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
			case 0: return "Принятая\nпоследовательность";
			case 1: return "Восстановленная\nпоследовательность";
			case 2: return "Декодированная\nпоследовательность";
			case 3: return "Количество искажённых\nпомехой бит";
			case 4: return "Количество верно исправленных\nбит в закодированной\nпоследовательности";
			case 5: return "Количество верно исправленных\nбит в декодированной\nпоследовательности";
			case 6: return "Количество ошибок в\nдекодированной\nпоследовательности";
			case 7: return "Доля ошибок в\nдекодированной\nпоследовательности";
			case 8: return "Количество верных\nсрабатываний проверочного бита";
			}
		}
		else {
			return QString::number(section + 1);
		}
	}
	return QVariant();
}

bool IterationResultTableModel::canFetchMore(const QModelIndex& parent) const
{
	return fetchedCount < results.size();
}

void IterationResultTableModel::fetchMore(const QModelIndex& parent)
{
	size_t remainder = results.size() - fetchedCount;
	size_t itemsToFetch = qMin(BATCH_SIZE, remainder);

	beginInsertRows(QModelIndex(), fetchedCount, fetchedCount + itemsToFetch - 1);
	// Nothing to do here as all the data is already generated
	fetchedCount += itemsToFetch;
	endInsertRows();
}