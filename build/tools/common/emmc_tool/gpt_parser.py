#!/usr/bin/python


# Author : n0fate
# E-Mail rapfer@gmail.com, n0fate@live.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# Using structures defined in Wikipedia('http://en.wikipedia.org/wiki/GUID_Partition_Table')


import getopt
import sys
import struct
import hashlib

import uuid

import zlib

LBA_SIZE = 512

PRIMARY_GPT_LBA = 1

OFFSET_CRC32_OF_HEADER = 16
GPT_HEADER_FORMAT = "<8sIIIIQQQQ16sQIII420x"
GUID_PARTITION_ENTRY_FORMAT = "<16s16sQQQ72s"


# gpt parser code start


def get_lba(fhandle, entry_number, count):
    fhandle.seek(LBA_SIZE * entry_number)
    fbuf = fhandle.read(LBA_SIZE * count)

    return fbuf


def unsigned32(n):
    return n & 0xFFFFFFFF


def get_gpt_header(fhandle, fbuf, lba):
    fbuf = get_lba(fhandle, lba, 1)

    gpt_header = struct.unpack(GPT_HEADER_FORMAT, fbuf[:512])

    crc32_header_value = calc_header_crc32(fbuf, gpt_header[2])

    return gpt_header, crc32_header_value, fbuf


def make_nop(byte):
    nop_code = 0x00
    pk_nop_code = struct.pack("=B", nop_code)
    nop = pk_nop_code * byte
    return nop


def calc_header_crc32(fbuf, header_size):

    nop = make_nop(4)

    clean_header = (
        fbuf[:OFFSET_CRC32_OF_HEADER]
        + nop
        + fbuf[OFFSET_CRC32_OF_HEADER + 4 : header_size]
    )

    crc32_header_value = unsigned32(zlib.crc32(clean_header))
    return crc32_header_value


def an_gpt_header(gpt_header, crc32_header_value):

    signature = gpt_header[0]
    revision = gpt_header[1]
    headersize = gpt_header[2]
    crc32_header = gpt_header[3]
    # reserved = gpt_header[4]
    currentlba = gpt_header[5]
    backuplba = gpt_header[6]
    first_use_lba_for_partitions = gpt_header[7]
    last_use_lba_for_partitions = gpt_header[8]
    disk_guid = uuid.UUID(bytes_le=gpt_header[9])
    part_entry_start_lba = gpt_header[10]
    num_of_part_entry = gpt_header[11]
    size_of_part_entry = gpt_header[12]
    crc32_of_partition_array = gpt_header[13]
    print("[+] Primary GPT header")
    print(" [-] Signature: %s" % signature)
    print(" [-] Revision: %d" % revision)
    print(" [-] Header Size: %d" % headersize)
    if crc32_header_value == crc32_header:
        print(
            " [-] CRC32 of header: %X (VALID) => Real: %X"
            % (
                crc32_header,
                crc32_header_value,
            )
        )
    else:
        print(
            " [-] WARNING!! CRC32 of header: %X (INVALID) => Real: %X"
            % (
                crc32_header,
                crc32_header_value,
            )
        )
    print(" [-] Current LBA: 0x%.8X" % currentlba)
    print(" [-] Backup LBA: 0x%.8X" % backuplba)
    print(" [-] First usable LBA for partitions: 0x%.8X" % first_use_lba_for_partitions)
    print(" [-] Last usable LBA for partitions: 0x%.8X" % last_use_lba_for_partitions)
    print(" [-] Disk GUID: %s" % str(disk_guid).upper())
    print(" [-] Partition entries starting LBA: 0x%.8X" % part_entry_start_lba)
    print(" [-] Number of partition entries: %d" % num_of_part_entry)
    print(" [-] Size of partition entry: 0x%.8X" % size_of_part_entry)
    print(" [-] CRC32 of partition array: 0x%.8X" % crc32_of_partition_array)


# get partition entry
def get_part_entry(fbuf, offset, size):
    return struct.unpack(GUID_PARTITION_ENTRY_FORMAT, fbuf[offset : offset + size])


def get_part_table_area(f, gpt_header):
    part_entry_start_lba = gpt_header[10]
    first_use_lba_for_partitions = gpt_header[7]
    fbuf = get_lba(
        f, part_entry_start_lba, first_use_lba_for_partitions - part_entry_start_lba
    )

    return fbuf


def part_attribute(value):
    if value == 0:
        return "System Partition"
    elif value == 2:
        return "Legacy BIOS Bootable"
    elif value == 60:
        return "Read-Only"
    elif value == 62:
        return "Hidden"
    elif value == 63:
        return "Do not automount"
    else:
        return "UNKNOWN"


def check_partition_guid_type(guid):
    if guid == "024DEE41-33E7-11D3-9D69-0008C781F39F":
        return "MBR partition scheme", "None"

    elif guid == "C12A7328-F81F-11D2-BA4B-00A0C93EC93B":
        return "EFI System partition", "None"

    elif guid == "21686148-6449-6E6F-744E-656564454649":
        return "BIOS Boot partition", "None"

    # Windows

    elif guid == "E3C9E316-0B5C-4DB8-817D-F92DF00215AE":
        return "Microsoft Reserved Partition", "Windows"

    elif guid == "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7":
        return "Basic data partition / Linux filesystem data", "Windows / Linux"

    elif guid == "5808C8AA-7E8F-42E0-85D2-E1E90434CFB3":
        return "Logical Disk Manager metadata partition", "Windows"

    elif guid == "AF9B60A0-1431-4F62-BC68-3311714A69AD":
        return "Logical Disk Manager data partition", "Windows"

    elif guid == "DE94BBA4-06D1-4D40-A16A-BFD50179D6AC":
        return "Windows Recovery Environment", "Windows"

    elif guid == "37AFFC90-EF7D-4E96-91C3-2D7AE055B174":
        return "IBM General Parallel File System (GPFS) partition", "Windows"

    elif guid == "DB97DBA9-0840-4BAE-97F0-FFB9A327C7E1":
        return "Cluster metadata partition", "Windows"

    # HP-UX
    elif guid == "75894C1E-3AEB-11D3-B7C1-7B03A0000000":
        return "Data partition", "HP-UX"

    elif guid == "E2A1E728-32E3-11D6-A682-7B03A0000000":
        return "Service partition", "HP-UX"

    # Linux
    elif guid == "0FC63DAF-8483-4772-8E79-3D69D8477DE4":
        return "Linux filesystem data", "Linux"

    elif guid == "A19D880F-05FC-4D3B-A006-743F0F84911E":
        return "RAID partition", "Linux"

    elif guid == "0657FD6D-A4AB-43C4-84E5-0933C84B4F4F":
        return "Swao partition", "Linux"

    elif guid == "E6D6D379-F507-44C2-A23C-238F2A3DF928":
        return "Logical Volume Manager (LVM) partition", "Linux"

    elif guid == "8DA63339-0007-60C0-C436-083AC8230908":
        return "Reserved", "Linux"

    # FreeBSD
    elif guid == "83BD6B9D-7F41-11DC-BE0B-001560B84F0F":
        return "Boot partition", "FreeBSD"

    elif guid == "516E7CB4-6ECF-11D6-8FF8-00022D09712B":
        return "Data partition", "FreeBSD"

    elif guid == "516E7CB5-6ECF-11D6-8FF8-00022D09712B":
        return "Swap partition", "FreeBSD"

    elif guid == "516E7CB6-6ECF-11D6-8FF8-00022D09712B":
        return "Unix File System(UFS) partition", "FreeBSD"

    elif guid == "516E7CB8-6ECF-11D6-8FF8-00022D09712B":
        return "Vinum volume manager partition", "FreeBSD"

    elif guid == "516E7CB8-6ECF-11D6-8FF8-00022D09712B":
        return "ZFS partition", "FreeBSD"

    # Mac OS X

    elif guid == "48465300-0000-11AA-AA11-00306543ECAC":
        return "Hierarchical File System Plus (HFS+) partition", "Mac OS X"

    elif guid == "55465300-0000-11AA-AA11-00306543ECAC":
        return "Apple UFS", "Mac OS X"

    elif guid == "6A898CC3-1DD2-11B2-99A6-080020736631":
        return "ZFS / /usr partition", "Mac OS X / Solaris"

    elif guid == "52414944-0000-11AA-AA11-00306543ECAC":
        return "Apple RAID partition", "Mac OS X"

    elif guid == "52414944-5F4F-11AA-AA11-00306543ECAC":
        return "Apple RAID partition, offline", "Mac OS X"

    elif guid == "426F6F74-0000-11AA-AA11-00306543ECAC":
        return "Apple Boot partition", "Mac OS X"

    elif guid == "4C616265-6C00-11AA-AA11-00306543ECAC":
        return "Apple Label", "Mac OS X"

    elif guid == "5265636F-7665-11AA-AA11-00306543ECAC":
        return "Apple TV Recovery partition", "Mac OS X"

    elif guid == "53746F72-6167-11AA-AA11-00306543ECAC":
        return "Apple Core Storage (i.e. Lion FileVault) partition", "Mac OS X"

    # Solaris

    elif guid == "6A82CB45-1DD2-11B2-99A6-080020736631":
        return "Boot partition", "Solaris"

    elif guid == "6A85CF4D-1DD2-11B2-99A6-080020736631":
        return "Root partition", "Solaris"

    elif guid == "6A87C46F-1DD2-11B2-99A6-080020736631":
        return "Swap partition", "Solaris"

    elif guid == "6A8B642B-1DD2-11B2-99A6-080020736631":
        return "Backup partition", "Solaris"

    # elif guid == '6A898CC3-1DD2-11B2-99A6-080020736631':
    #    return '/usr partition', 'Solaris'

    elif guid == "6A8EF2E9-1DD2-11B2-99A6-080020736631":
        return "/var partition", "Solaris"

    elif guid == "6A90BA39-1DD2-11B2-99A6-080020736631":
        return "/home partition", "Solaris"

    elif guid == "6A9283A5-1DD2-11B2-99A6-080020736631":
        return "Alternate sector", "Solaris"

    elif (
        guid == "6A945A3B-1DD2-11B2-99A6-080020736631"
        or guid == "6A9630D1-1DD2-11B2-99A6-080020736631"
        or guid == "6A980767-1DD2-11B2-99A6-080020736631"
        or guid == "6A96237F-1DD2-11B2-99A6-080020736631"
        or guid == "6A8D2AC7-1DD2-11B2-99A6-080020736631"
    ):
        return "Reserved partition", "Solaris"

    # NetBSD

    elif guid == "49F48D32-B10E-11DC-B99B-0019D1879648":
        return "Swap partition", "NetBSD"

    elif guid == "49F48D5A-B10E-11DC-B99B-0019D1879648":
        return "FFS partition", "NetBSD"

    elif guid == "49F48D82-B10E-11DC-B99B-0019D1879648":
        return "LFS partition", "NetBSD"

    elif guid == "49F48DAA-B10E-11DC-B99B-0019D1879648":
        return "RAID partition", "NetBSD"

    elif guid == "2DB519C4-B10F-11DC-B99B-0019D1879648":
        return "Concatenated partition", "NetBSD"

    elif guid == "2DB519EC-B10F-11DC-B99B-0019D1879648":
        return "Encrypted partition", "NetBSD"

    # Chrome OS

    elif guid == "FE3A2A5D-4F32-41A7-B725-ACCC3285A309":
        return "ChromeOS kernel", "Chrome OS"

    elif guid == "3CB8E202-3B7E-47DD-8A3C-7FF2A13CFCEC":
        return "ChromeOS rootfs", "Chrome OS"

    elif guid == "2E0A753D-9E48-43B0-8337-B15192CB1B5E":
        return "ChromeOS future use", "Chrome OS"

    # VMware ESX

    elif guid == "AA31E02A-400F-11DB-9590-000C2911D1B8":
        return "VMFS partition", "VMware ESX"

    elif guid == "9D275380-40AD-11DB-BF97-000C2911D1B8":
        return "vmkcore crash partition", "VMware ESX"

    # Midnight BSD

    elif guid == "85D5E45E-237C-11E1-B4B3-E89A8F7FC3A7":
        return "Boot partition", "MidnightBSD"

    elif guid == "85D5E45A-237C-11E1-B4B3-E89A8F7FC3A7":
        return "Data partition", "MidnightBSD"

    elif guid == "85D5E45B-237C-11E1-B4B3-E89A8F7FC3A7":
        return "Swap partition", "MidnightBSD"

    elif guid == "0394Ef8B-237E-11E1-B4B3-E89A8F7FC3A7":
        return "Unix File System (UFS) partition", "MidnightBSD"

    elif guid == "85D5E45C-237C-11E1-B4B3-E89A8F7FC3A7":
        return "Vinum volume manager partition", "MidnightBSD"

    elif guid == "85D5E45D-237C-11E1-B4B3-E89A8F7FC3A7":
        return "ZFS partition", "MidnightBSD"

    # else
    else:
        return "unknown partition", "UNKNOWN"


# analysis partition table
def an_part_table(partition_table, gpt_header):
    num_of_part_entry = gpt_header[11]
    size_of_part_entry = gpt_header[12]
    crc32_of_partition_array = gpt_header[13]

    part_list = []

    crc32_part_value = unsigned32(zlib.crc32(partition_table))
    print("")
    print("[+] Partition table")
    if crc32_part_value == crc32_of_partition_array:
        print(" [-] CRC32 Check : %.8X (VALID)" % crc32_part_value)
    else:
        print(" [-] WARNING!! CRC32 Check : %.8X (INVALID)" % crc32_part_value)

    for part_entry_num in range(0, num_of_part_entry):
        part_entry = get_part_entry(
            partition_table, size_of_part_entry * part_entry_num, size_of_part_entry
        )

        # first LBA, last LBA
        if part_entry[2] == 0 or part_entry[3] == 0:
            continue

        part_list.append(part_entry)

    count = 1
    for part_entry in part_list:
        print("")
        print(" [-] Partition %d" % count)
        print(
            "  [-] Partition type GUID: %s"
            % str(uuid.UUID(bytes_le=part_entry[0])).upper()
        )
        print(
            "      => Partition type: %s, %s"
            % (
                check_partition_guid_type(
                    str(uuid.UUID(bytes_le=part_entry[0])).upper()
                )
            )
        )
        print(
            "  [-] Unique partition GUID: %s"
            % str(uuid.UUID(bytes_le=part_entry[1])).upper()
        )
        print("  [-] First LBA: %d" % part_entry[2])
        print("      => Disk Offset: 0x%.8X" % (part_entry[2] * LBA_SIZE))
        print("  [-] Last LBA: %d" % part_entry[3])
        print("      => Disk Offset: 0x%.8X" % (part_entry[3] * LBA_SIZE))
        print(
            "  [-] Partition size: %dMB"
            % ((part_entry[3] - part_entry[2]) * LBA_SIZE / 1024 / 1024)
        )
        print(
            "  [-] Attribute flags: %d, %s"
            % (
                part_entry[4],
                part_attribute(part_entry[4]),
            )
        )
        print("  [-] Partition Name: %s" % str(part_entry[5]))
        count += 1


def usage(argv):
    print("%s <DISK IMAGE>" % argv[0])
    sys.exit()


def main():

    try:
        option, args = getopt.getopt(sys.argv[1:], "")

    except getopt.GetoptError:
        usage(sys.argv)
        sys.exit()

    try:
        if len(sys.argv) != 2:
            usage(sys.argv)
            sys.exit()

    except IndexError:
        usage()
        sys.exit()

    try:
        f = open(sys.argv[1], "rb")
    except IOError:
        print("[+] WARNING!! Can not open disk image.")
        # usage(sys.argv)
        sys.exit()

    fbuf = ""

    # Protected MBR
    # You can use mbr_parser.py at http://gleeda.blogspot.com/2012/04/mbr-parser.html

    # Primary GPT header
    gpt_header, crc32_header_value, gpt_buf = get_gpt_header(f, fbuf, PRIMARY_GPT_LBA)
    an_gpt_header(gpt_header, crc32_header_value)

    h = hashlib.md5()
    h.update(gpt_buf)
    print("")
    print("[+] Primary GPT header md5: %s" % h.hexdigest())

    print("")

    # Partition entries
    fbuf = get_part_table_area(f, gpt_header)
    an_part_table(fbuf, gpt_header)

    h = hashlib.md5()
    h.update(fbuf)
    print("")
    print("[+] Partition table md5: %s" % h.hexdigest())

    # backup GPT header
    print("")
    try:
        gpt_header, crc32_header_value, gpt_buf = get_gpt_header(f, fbuf, gpt_header[6])
        an_gpt_header(gpt_header, crc32_header_value)

        h = hashlib.md5()
        h.update(gpt_buf)
        print("")
        print("[+] Backup GPT header md5: %s" % h.hexdigest())
    except struct.error:
        print("[+] WARNING!! Backup GPT header can not found. Check your disk image.")
        print(
            "[+] WARNING!! Backup GPT header offset: 0x%.8X"
            % (gpt_header[6] * LBA_SIZE)
        )

    f.close()


if __name__ == "__main__":
    main()
