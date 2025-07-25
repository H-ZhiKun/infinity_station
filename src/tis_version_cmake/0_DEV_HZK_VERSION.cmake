if(__CONFIG_INCLUDED_${CURRENT_VERSION})
    return()
endif()
set(__CONFIG_INCLUDED_${CURRENT_VERSION} TRUE)

# HZK开发版本
list(APPEND MODS_CONFIGS "qml_engine" "database_manager" "hardware_manager" "remote_access")
list(APPEND CTLS_CONFIGS "unit_test/hzk_ctls")