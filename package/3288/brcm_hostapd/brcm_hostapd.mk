
BRCM_HOSTAPD_SITE = https://github.com/Infineon/ifx-hostap/archive/refs/tags
BRCM_HOSTAPD_SOURCE = release-2_11-2024_0514.tar.gz

BRCM_HOSTAPD_DEPENDENCIES = host-pkgconf libnl libopenssl

BRCM_HOSTAPD_SUBDIR = hostapd
BRCM_HOSTAPD_CONFIG = $(BRCM_HOSTAPD_DIR)/$(BRCM_HOSTAPD_SUBDIR)/.config
BRCM_HOSTAPD_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
BRCM_HOSTAPD_LIBS = `$(PKG_CONFIG_HOST_BINARY) --libs openssl`

define BRCM_HOSTAPD_CONFIGURE_CMDS
	cp $(@D)/$(BRCM_HOSTAPD_SUBDIR)/defconfig_base $(BRCM_HOSTAPD_CONFIG)
endef

# LIBS for hostapd, LIBS_c for hostapd_cli
define BRCM_HOSTAPD_BUILD_CMDS
	$(TARGET_MAKE_ENV) CFLAGS="$(BRCM_HOSTAPD_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" BINDIR=/usr/sbin \
		LIBS="$(BRCM_HOSTAPD_LIBS)" LIBS_c="$(BRCM_HOSTAPD_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(BRCM_HOSTAPD_SUBDIR)
endef

define BRCM_HOSTAPD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(BRCM_HOSTAPD_SUBDIR)/hostapd \
		$(TARGET_DIR)/usr/sbin/hostapd
	$(INSTALL) -m 0755 -D $(@D)/$(BRCM_HOSTAPD_SUBDIR)/hostapd_cli \
		$(TARGET_DIR)/usr/sbin/hostapd_cli
endef

$(eval $(generic-package))
