include $(TOPDIR)/rules.mk

PKG_NAME:=tunet
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Robert Ying <yingrui205@gmail.com>
PKG_LICENSE:=MIT

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/robertying/tunet-c/releases/download/v$(PKG_VERSION)
PKG_HASH:=517432b93e35ea0f41cd6147cb1c74937d70c26ac47c278f175a21c336be2453

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/tunet/Default
  SECTION:=utils
  CATEGORY:=Network
  TITLE:=Tsinghua University login utility
  URL:=https://github.com/robertying/tunet-c
endef

define Package/tunet
  $(call Package/tunet/Default)
  DEPENDS:= +libtunet
endef

define Package/libtunet
  $(call Package/tunet/Default)
  DEPENDS:= +libopenssl +libcurl
endef

define Package/tunet/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/tunet $(1)/usr/bin/
endef

define Package/libtunet/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtunet*.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,tunet))
$(eval $(call BuildPackage,libtunet))
