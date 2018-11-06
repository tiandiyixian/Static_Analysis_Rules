static void
dissect_amqp_0_10_struct_fragment_properties(tvbuff_t *tvb,
                                             packet_info *pinfo,
                                             proto_tree *tree,
                                             int offset,
                                             guint32 struct_length)
{
    proto_item *args_tree;
    proto_item *flags_item;
    guint8      flag1, flag2;
    int         flags_offset;
    int         max_length;

    max_length = offset + struct_length;
    args_tree = proto_item_add_subtree(tree, ett_args);
    AMQP_INCREMENT(offset, 2, max_length);  /
    flags_offset = offset;
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    flags_item = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_argument_packing_flags,
                                     tvb, offset, 2, ENC_BIG_ENDIAN);
    if ((flag1 & ~0x07) || (flag2 != 0))
        expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
    AMQP_INCREMENT(offset, 2, max_length);

    /
    proto_tree_add_item(args_tree,
                        hf_amqp_0_10_struct_fragment_properties_first,
                        tvb, flags_offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(args_tree,
                        hf_amqp_0_10_struct_fragment_properties_last,
                        tvb, flags_offset, 1, ENC_BIG_ENDIAN);
    if (flag1 & 0x04) {
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_struct_fragment_properties_size,
                            tvb, offset, 8, ENC_BIG_ENDIAN);
        AMQP_INCREMENT(offset, 8, max_length);
    }
}