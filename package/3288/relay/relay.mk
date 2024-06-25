RELAY_SITE = $(TOPDIR)/app/relay
RELAY_SITE_METHOD = local

define RELAY_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 package/3288/relay/S20relay $(TARGET_DIR)/etc/init.d/S20relay
endef

$(eval $(cmake-package))
