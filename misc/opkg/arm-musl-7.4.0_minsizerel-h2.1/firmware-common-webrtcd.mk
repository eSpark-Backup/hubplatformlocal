define Package/firmware-common-webrtcd
	$(call Package/hub-platform/Default)
	TITLE=firmware-common-webrtcd
	VERSION=1.0
endef

define Package/firmware-common-webrtcd/description
	Just firmware-common-webrtcd package
endef

define Package/firmware-common-webrtcd/install
	$(INSTALL_DIR) $(1)/
	$(CP) $(PKG_INSTALL_DIR)/firmware-common-webrtcd/* $(1)/
endef

$(eval $(call BuildPackage,firmware-common-webrtcd))
