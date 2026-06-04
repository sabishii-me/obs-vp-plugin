#include "settings-dialog.hpp"
#include "vp-settings.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("VP Plugin Settings");
	setMinimumWidth(480);

	auto *layout = new QVBoxLayout(this);

	// Workspace root row
	auto *rowLayout = new QHBoxLayout();
	rowLayout->addWidget(new QLabel("Workspace Root:"));
	m_pathEdit = new QLineEdit(
		QString::fromStdString(vp_settings_get_workspace_root()));
	m_pathEdit->setPlaceholderText("Select output root folder...");
	rowLayout->addWidget(m_pathEdit, 1);
	auto *browseBtn = new QPushButton("Browse...");
	connect(browseBtn, &QPushButton::clicked, this,
		&SettingsDialog::onBrowse);
	rowLayout->addWidget(browseBtn);
	layout->addLayout(rowLayout);

	// OK / Cancel
	auto *buttons = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons, &QDialogButtonBox::accepted, this,
		&SettingsDialog::onAccepted);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
	layout->addWidget(buttons);
}

void SettingsDialog::onBrowse()
{
	QString dir = QFileDialog::getExistingDirectory(
		this, "Select Workspace Root",
		m_pathEdit->text().isEmpty() ? QString() : m_pathEdit->text());
	if (!dir.isEmpty())
		m_pathEdit->setText(dir);
}

void SettingsDialog::onAccepted()
{
	vp_settings_set_workspace_root(m_pathEdit->text().toStdString());
	vp_settings_save();
	accept();
}
