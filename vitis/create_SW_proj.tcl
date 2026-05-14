# -------------------------------
# Basic config
# -------------------------------
set arch 64-bit
set processor_name psu_cortexa53_0
set os_name standalone

set project_path ./sdk_workspace
set hardware_path ./xsa

set platform_name system_wrapper
set domain_name ${os_name}_domain
set project_name vitis_proj

set hardware_name ${hardware_path}/${platform_name}.xsa

# -------------------------------
# Set workspace
# -------------------------------
setws -switch ${project_path}
getws

# -------------------------------
# Create platform & domain ONCE
# -------------------------------
set platform_dir "${project_path}/${platform_name}"

if {![file exists $platform_dir]} {
    puts "Creating platform and domain (first time)"

    platform create -name ${platform_name} -hw ${hardware_name}
    domain create   -name ${domain_name} -os ${os_name} -proc ${processor_name}

    platform generate -domains
    platform write
} else {
    puts "Platform already exists, reuse it"
}

# -------------------------------
# Activate platform & domain
# -------------------------------
platform active ${platform_name}
domain   active ${domain_name}

# -------------------------------
# BSP configuration (SAFE)
# -------------------------------
bsp config stdin  psu_uart_0
bsp config stdout psu_uart_0

bsp setlib -name xilffs
bsp config fs_interface 1

bsp setlib -name lwip211
bsp config lwip_dhcp false
bsp config phy_link_speed CONFIG_LINKSPEED1000

bsp regenerate

platform write

# -------------------------------
# Create app if not exists
# -------------------------------
set app_dir "${project_path}/${project_name}"

if {![file exists $app_dir]} {
    puts "Creating application ${project_name}"

    app create \
        -name ${project_name} \
        -platform ${platform_name} \
        -domain ${domain_name} \
        -proc ${processor_name} \
        -os ${os_name} \
        -template {Empty Application}

    app config -name ${project_name} \
        -set linker-misc {-Wl,-Map,${ProjName}.map}
} else {
    puts "Application already exists, skip create"
}

# -------------------------------
# Import sources
# -------------------------------
puts "Import application sources"

importsources \
    -name ${project_name} \
    -path [pwd]/src/${project_name}/src \
    -linker-script

exit