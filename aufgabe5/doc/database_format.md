The file storing the family tree should end in .dft (date family tree).

DTF File:
    File Header (.header, 0x0000):
        | Address | Offset | Width | Description | Notes                                                                 |
        | ------- | ------ | ----- | ----------- | --------------------------------------------------------------------- |
        | 0x0000  | 0      | 4     | version     | Must be 1 for the version described in this file.                     |
        | 0x0004  | 4      | 4     | length      | Amount of GeneticEntry objects stored                                 |
        | 0x0008  | 8      | 4     | strtab_len  | Length (in bytes) of the string table                                 |
        | 0x000c  | 12     | 4     | checksum    | Set so that the sum of the whole file as 4 byte chunks, is equal to 0 |

    Genetic Entry Table (.gentab, len(.header)):
        Genetic Entry Table Entry (.gentab_entry, len(.header) + offset):
            | Address | Offset | Width | Description | Notes                                                                 |
            | ------- | ------ | ----- | ----------- | --------------------------------------------------------------------- |
            | 0x00000 | 0      | 4     | uniq_id     | Unique identifier, identifiying the genetic entry                     |
            | 0x00004 | 4      | 4     | mother_addr | Address of the mother, zero means missing                             |
            | 0x00008 | 8      | 4     | father_addr | Address of the father, zero means missing                             |
            | 0x0000c | 12     | 4     | name_addr   | Address of the name, zero means missing                               |

    String Table (.strtab, len(.header) + len(.gentab)):
        String Table Entry (.strtab_entry, len(.header) + len(.gentab) + offset):
            | Address | Offset | Width | Description | Notes                            |
            | ------- | ------ | ----- | ----------- | -------------------------------- |
            | 0x00000 | 0      | 4     | str_len     | Length of the string             |
            | 0x00004 | 4      | n     | str_data    | String data, not null terminated |