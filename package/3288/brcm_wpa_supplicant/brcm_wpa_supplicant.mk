
BRCM_WPA_SUPPLICANT_SITE = https://github.com/Infineon/ifx-hostap/archive/refs/tags
BRCM_WPA_SUPPLICANT_SOURCE = release-2_11-2024_0514.tar.gz

BRCM_WPA_SUPPLICANT_DEPENDENCIES = host-pkgconf libnl libopenssl

BRCM_WPA_SUPPLICANT_SUBDIR = wpa_supplicant
BRCM_WPA_SUPPLICANT_CONFIG = $(BRCM_WPA_SUPPLICANT_DIR)/$(BRCM_WPA_SUPPLICANT_SUBDIR)/.config
BRCM_WPA_SUPPLICANT_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
BRCM_WPA_SUPPLICANT_LIBS = `$(PKG_CONFIG_HOST_BINARY) --libs openssl`

define BRCM_WPA_SUPPLICANT_CONFIGURE_CMDS
	cp $(@D)/$(BRCM_WPA_SUPPLICANT_SUBDIR)/defconfig_base $(BRCM_WPA_SUPPLICANT_CONFIG)
endef

# LIBS for wpa_supplicant, LIBS_c for wpa_cli
define BRCM_WPA_SUPPLICANT_BUILD_CMDS
	$(TARGET_MAKE_ENV) CFLAGS="$(BRCM_WPA_SUPPLICANT_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" BINDIR=/usr/sbin \
		LIBS="$(BRCM_WPA_SUPPLICANT_LIBS)" LIBS_c="$(BRCM_WPA_SUPPLICANT_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(BRCM_WPA_SUPPLICANT_SUBDIR)
endef

define BRCM_WPA_SUPPLICANT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(BRCM_WPA_SUPPLICANT_SUBDIR)/wpa_supplicant \
		$(TARGET_DIR)/usr/sbin/wpa_supplicant
	$(INSTALL) -m 0755 -D $(@D)/$(BRCM_WPA_SUPPLICANT_SUBDIR)/wpa_cli \
		$(TARGET_DIR)/usr/sbin/wpa_cli
endef

$(eval $(generic-package))
