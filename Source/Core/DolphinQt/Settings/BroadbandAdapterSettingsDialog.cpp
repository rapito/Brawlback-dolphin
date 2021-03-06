// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Settings/BroadbandAdapterSettingsDialog.h"

#include <regex>
#include <string>

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>

#include "Common/StringUtil.h"
#include "Core/ConfigManager.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"

BroadbandAdapterSettingsDialog::BroadbandAdapterSettingsDialog(QWidget* parent, Type bba_type)
    : QDialog(parent)
{
  m_bba_type = bba_type;
  InitControls();
}

void BroadbandAdapterSettingsDialog::InitControls()
{
  QLabel* address_label = nullptr;
  QLabel* description = nullptr;
  QString address_placeholder;
  QString current_address;
  QString window_title;

  switch (m_bba_type)
  {
  case Type::Ethernet:
    // i18n: MAC stands for Media Access Control. A MAC address uniquely identifies a network
    // interface (physical) like a serial number. "MAC" should be kept in translations.
    address_label = new QLabel(tr("Enter new Broadband Adapter MAC address:"));
    address_placeholder = QString::fromStdString("aa:bb:cc:dd:ee:ff");
    current_address = QString::fromStdString(SConfig::GetInstance().m_bba_mac);
    description = new QLabel(tr("For setup instructions, <a "
                                "href=\"https://wiki.dolphin-emu.org/"
                                "index.php?title=Broadband_Adapter\">refer to this page</a>."));

    // i18n: MAC stands for Media Access Control. A MAC address uniquely identifies a network
    // interface (physical) like a serial number. "MAC" should be kept in translations.
    window_title = tr("Broadband Adapter MAC Address");
    break;

  case Type::XLinkKai:
    address_label = new QLabel(tr("Enter IP address of device running the XLink Kai Client:"));
    address_placeholder = QString::fromStdString("127.0.0.1");
    current_address = QString::fromStdString(SConfig::GetInstance().m_bba_xlink_ip);
    description =
        new QLabel(tr("For setup instructions, <a "
                      "href=\"https://www.teamxlink.co.uk/wiki/Dolphin\">refer to this page</a>."));
    window_title = tr("XLink Kai BBA Destination Address");
    break;
  }

  setWindowTitle(window_title);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  m_address_input = new QLineEdit(current_address);
  m_address_input->setPlaceholderText(address_placeholder);

  auto buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonbox, &QDialogButtonBox::accepted, this,
          &BroadbandAdapterSettingsDialog::SaveAddress);
  connect(buttonbox, &QDialogButtonBox::rejected, this, &BroadbandAdapterSettingsDialog::reject);

  description->setTextFormat(Qt::RichText);
  description->setWordWrap(true);
  description->setTextInteractionFlags(Qt::TextBrowserInteraction);
  description->setOpenExternalLinks(true);

  auto* main_layout = new QVBoxLayout();
  main_layout->addWidget(address_label);
  main_layout->addWidget(m_address_input);
  main_layout->addWidget(description);
  main_layout->addWidget(buttonbox);

  setLayout(main_layout);
}

void BroadbandAdapterSettingsDialog::SaveAddress()
{
  const std::string bba_new_address(StripSpaces(m_address_input->text().toStdString()));

  switch (m_bba_type)
  {
  case Type::Ethernet:
    if (!std::regex_match(bba_new_address, std::regex("([0-9A-Fa-f]{2}:){5}([0-9A-Fa-f]{2})")))
    {
      ModalMessageBox::critical(
          this, tr("Broadband Adapter Error"),
          // i18n: MAC stands for Media Access Control. A MAC address uniquely identifies a network
          // interface (physical) like a serial number. "MAC" should be kept in translations.
          tr("The entered MAC address is invalid."));
      return;
    }
    SConfig::GetInstance().m_bba_mac = bba_new_address;
    break;

  case Type::XLinkKai:
    SConfig::GetInstance().m_bba_xlink_ip = bba_new_address;
    break;
  }

  accept();
}
