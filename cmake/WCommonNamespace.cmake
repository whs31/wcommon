include_guard(GLOBAL)

# The sources are spelled in this namespace. A build in any other namespace compiles a respelled copy
# of them, see wcommon_rewrite_namespace().
set(WCOMMON_SOURCE_NAMESPACE "whs31")

# Derives from a C++ namespace such as `whs31` or `acme::util` its include path (`acme/util`) and its
# macro prefix (`ACME_UTIL`).
function(wcommon_namespace_properties namespace out_path out_macro_prefix)
  if(NOT namespace MATCHES "^[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*$")
    message(FATAL_ERROR
      "Invalid WCommon namespace '${namespace}': expected identifiers joined by '::', "
      "e.g. 'whs31' or 'acme::util'"
    )
  endif()
  string(REPLACE "::" "/" path "${namespace}")
  string(REPLACE "::" "_" macro_prefix "${namespace}")
  string(TOUPPER "${macro_prefix}" macro_prefix)
  set(${out_path} "${path}" PARENT_SCOPE)
  set(${out_macro_prefix} "${macro_prefix}" PARENT_SCOPE)
endfunction()

# wcommon_rewrite_namespace(
#   SOURCE_DIR <dir> DESTINATION_DIR <dir> NAMESPACE <namespace>
#   FILES <relative paths>... [OUTPUT_VARIABLE <var>]
# )
#
# Copies FILES from SOURCE_DIR to DESTINATION_DIR, respelling WCOMMON_SOURCE_NAMESPACE as NAMESPACE:
# qualified names, namespace declarations, include paths (also in the copied paths themselves) and
# the macro prefix. A copy is written only when its content changes, so a reconfiguration rebuilds
# only what was edited, and copies of removed files are deleted. Editing a source reconfigures the
# build. OUTPUT_VARIABLE receives the copied paths, relative to DESTINATION_DIR.
function(wcommon_rewrite_namespace)
  cmake_parse_arguments(PARSE_ARGV 0 arg "" "SOURCE_DIR;DESTINATION_DIR;NAMESPACE;OUTPUT_VARIABLE" "FILES")
  set(from "${WCOMMON_SOURCE_NAMESPACE}")
  set(to "${arg_NAMESPACE}")
  wcommon_namespace_properties("${from}" from_path from_prefix)
  wcommon_namespace_properties("${to}" to_path to_prefix)

  set(identifier "A-Za-z0-9_")
  set(outputs)
  foreach(file IN LISTS arg_FILES)
    set(input "${arg_SOURCE_DIR}/${file}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${input}")
    file(READ "${input}" content)

    # The leading newline lets every pattern require a non-identifier character before its match.
    set(content "\n${content}")
    string(REGEX REPLACE "([^${identifier}/])${from_path}/common/" "\\1${to_path}/common/" content "${content}")
    string(REGEX REPLACE "([^${identifier}])${from}::" "\\1${to}::" content "${content}")
    string(REGEX REPLACE "([^${identifier}])namespace ${from}([^${identifier}:])" "\\1namespace ${to}\\2" content "${content}")
    string(REGEX REPLACE "([^${identifier}])${from_prefix}_" "\\1${to_prefix}_" content "${content}")
    string(SUBSTRING "${content}" 1 -1 content)

    string(REGEX REPLACE "(^|/)${from_path}/common/" "\\1${to_path}/common/" output "${file}")
    list(APPEND outputs "${output}")
    set(output "${arg_DESTINATION_DIR}/${output}")
    if(EXISTS "${output}")
      file(READ "${output}" previous)
      if(previous STREQUAL content)
        continue()
      endif()
    endif()
    file(WRITE "${output}" "${content}")
  endforeach()

  file(GLOB_RECURSE existing RELATIVE "${arg_DESTINATION_DIR}" "${arg_DESTINATION_DIR}/*")
  foreach(file IN LISTS existing)
    if(NOT file IN_LIST outputs)
      file(REMOVE "${arg_DESTINATION_DIR}/${file}")
    endif()
  endforeach()

  if(arg_OUTPUT_VARIABLE)
    set(${arg_OUTPUT_VARIABLE} "${outputs}" PARENT_SCOPE)
  endif()
endfunction()
