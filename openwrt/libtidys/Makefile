include $(TOPDIR)/rules.mk

PKG_NAME:=libtidys
PKG_VERSION:=5.7.28
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libtidys
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=The granddaddy of HTML tools, with support for modern standards
	URL:=https://www.html-tidy.org
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ../../tidy-html5/* $(PKG_BUILD_DIR)/
endef

define Package/libtidys/install
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtidys.{a,so*} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtidys))
