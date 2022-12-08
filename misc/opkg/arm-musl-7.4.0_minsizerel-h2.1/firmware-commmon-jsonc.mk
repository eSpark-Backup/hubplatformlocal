define Package/firmware-commmon-jsonc
	$(call Package/hub-platform/Default)
	TITLE=firmware-commmon-jsonc
	VERSION=1.0
	EXTRA_DEPENDS += libc (=1.1.23-2),
	EXTRA_DEPENDS += libgcc (=7.4.0-2),
	DEPENDS += +libc
	DEPENDS += +libgcc
endef

define Package/firmware-commmon-jsonc/description
	Just firmware-commmon-jsonc package
endef

define Package/firmware-commmon-jsonc/install
	$(INSTALL_DIR) $(1)/
	$(CP) $(PKG_INSTALL_DIR)/firmware-commmon-jsonc/* $(1)/
endef

$(eval $(call BuildPackage,firmware-commmon-jsonc))
