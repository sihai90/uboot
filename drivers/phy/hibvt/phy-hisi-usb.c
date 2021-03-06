/*
* Copyright (c) 2018 HiSilicon Technologies Co., Ltd.
*
* This program is free software; you can redistribute  it and/or modify it
* under  the terms of  the GNU General Public License as published by the
* Free Software Foundation;  either version 2 of the  License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#if defined(CONFIG_TARGET_HI3559AV100)
#include "phy-usb-hi3559av100.c"
#endif
#if defined(CONFIG_TARGET_HI3556AV100)
#include "phy-usb-hi3556av100.c"
#endif
#if defined(CONFIG_TARGET_HI3519AV100)
#include "phy-usb-hi3519av100.c"
#endif
#if defined(CONFIG_TARGET_HI3516CV500)
#include "phy-usb-hi3516cv500.c"
#endif
#if (defined(CONFIG_TARGET_HI3516DV300) || defined(CONFIG_TARGET_HI3516AV300))
#include "phy-usb-hi3516dv300.c"
#endif
#if defined(CONFIG_TARGET_HI3556V200)
#include "phy-usb-hi3556v200.c"
#endif
#if defined(CONFIG_TARGET_HI3559V200)
#include "phy-usb-hi3559v200.c"
#endif
#ifdef CONFIG_TARGET_HI3516EV200
#include "phy-hi3516ev200-usb.c"
#endif
#ifdef CONFIG_TARGET_HI3516EV300
#include "phy-hi3516ev300-usb.c"
#endif
#ifdef CONFIG_TARGET_HI3518EV300
#include "phy-hi3518ev300-usb.c"
#endif
#ifdef CONFIG_TARGET_HI3516DV200
#include "phy-hi3516dv200-usb.c"
#endif
