include $(TOPDIR)/rules.mk

PKG_NAME:=libtidy
PKG_VERSION:=5.7.28
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libtidy
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=tidy - HTML syntax checker
	URL:=https://www.html-tidy.org
endef

PKG_INSTALL:=1

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ../../tidy-html5/* $(PKG_BUILD_DIR)/
endef

define Package/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.a $(1)/usr/lib/
endef

define Package/libtidy/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtidy))
