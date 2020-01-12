include $(TOPDIR)/rules.mk

PKG_NAME:=libtidy
PKG_VERSION:=5.7.28
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/robertying/tidy-html5.git
PKG_SOURCE_VERSION:=7d7f86ea93fa5741a6f6e36bde76af8f5c8e803b
PKG_MIRROR_HASH:=skip

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libtidy
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=tidy - HTML syntax checker
	URL:=https://www.html-tidy.org
endef

PKG_INSTALL:=1

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig $(1)/usr/lib/
endef

define Package/libtidy/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtidy))
