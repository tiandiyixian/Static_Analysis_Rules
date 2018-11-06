static void
dissect_hsdsch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                            int offset, struct fp_info *p_fp_info)
{
    gboolean is_control_frame;
    guint header_length = 0;
    guint16 header_crc = 0;
    proto_item * header_crc_pi = NULL;

    /
    header_crc = tvb_get_bits8(tvb, 0, 7);
    header_crc_pi = proto_tree_add_item(tree, hf_fp_header_crc, tvb, offset, 1, ENC_BIG_ENDIAN);

    /
    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;
    proto_tree_add_item(tree, hf_fp_ft, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO, is_control_frame ? " [Control] " : " [Data] ");

    if (is_control_frame) {
        dissect_common_control(tvb, pinfo, tree, offset, p_fp_info);
        /
        if (preferences_header_checksum) {
            verify_control_frame_crc(tvb, pinfo, header_crc_pi, header_crc);
        }
    }
    else {
        guint8 number_of_pdus;
        guint16 pdu_length;
        guint16 user_buffer_size;
        int i;
        umts_mac_info *macinf;
        rlc_info *rlcinf;

        rlcinf = (rlc_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0);
        macinf = (umts_mac_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0);

        /
        /

        /
        if ((p_fp_info->release == 6) ||
            (p_fp_info->release == 7)) {

            guint8 frame_seq_no = (tvb_get_guint8(tvb, offset) & 0xf0) >> 4;
            proto_tree_add_item(tree, hf_fp_frame_seq_nr, tvb, offset, 1, ENC_BIG_ENDIAN);

            col_append_fstr(pinfo->cinfo, COL_INFO, "  seqno=%u", frame_seq_no);
        }

        /
        proto_tree_add_item(tree, hf_fp_cmch_pi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /
        pdu_length = (tvb_get_ntohs(tvb, offset) >> 3);
        proto_tree_add_item(tree, hf_fp_mac_d_pdu_len, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;
        macinf->pdu_len = pdu_length;

        if ((p_fp_info->release == 6) ||
            (p_fp_info->release == 7)) {

            /
            proto_tree_add_item(tree, hf_fp_flush, tvb, offset-1, 1, ENC_BIG_ENDIAN);

            /
            proto_tree_add_item(tree, hf_fp_fsn_drt_reset, tvb, offset-1, 1, ENC_BIG_ENDIAN);
        }


        /
        number_of_pdus = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_num_of_pdu, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /
        user_buffer_size = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(tree, hf_fp_user_buffer_size, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;

        header_length = offset;


        /
        /
        for (i=0;i<number_of_pdus; i++) {
            macinf->content[i] = hsdsch_macdflow_id_mac_content_map[p_fp_info->hsdsch_macflowd_id]; /
            macinf->lchid[i] = fake_lchid_macd_flow[p_fp_info->hsdsch_macflowd_id];/
            macinf->fake_chid[i] = TRUE;    /
            macinf->macdflow_id[i] = p_fp_info->hsdsch_macflowd_id;    /
            /
            rlcinf->mode[i] = hsdsch_macdflow_id_rlc_map[p_fp_info->hsdsch_macflowd_id];


            /
            if ( / p_fp_info->hsdhsch_macfdlow_is_mux[p_fp_info->hsdsch_macflowd_id] ) {
                macinf->ctmux[i] = TRUE;
            } else if (p_fp_info->hsdsch_macflowd_id == 0) {              /
                expert_add_info(pinfo, NULL, &ei_fp_maybe_srb);
            } else {
                    macinf->ctmux[i] = FALSE;    /
            }
            rlcinf->urnti[i] = p_fp_info->com_context_id;
            rlcinf->li_size[i] = RLC_LI_7BITS;
            rlcinf->deciphered[i] = FALSE;
            rlcinf->ciphered[i] = FALSE;
            rlcinf->rbid[i] = macinf->lchid[i];

#if 0
            /             * This needs more work though since we must figure out when the re-configuration becomes
             * active based on the CFN value
             * */
            /
            if (p_fp_info->reset_frag) {
                rlc_reset_channel(rlcinf->mode[i], macinf->lchid[i], p_fp_info->is_uplink,  rlcinf->urnti[i] );
                p_fp_info->reset_frag = FALSE;

            }
#endif
        }


        /
        offset = dissect_macd_pdu_data(tvb, pinfo, tree, offset, pdu_length,
                                       number_of_pdus, p_fp_info);

        col_append_fstr(pinfo->cinfo, COL_INFO, "  %ux%u-bit PDUs  User-Buffer-Size=%u",
                        number_of_pdus, pdu_length, user_buffer_size);

        /
        if (((p_fp_info->release == 6) ||
             (p_fp_info->release == 7)) &&
            (tvb_reported_length_remaining(tvb, offset) > 2)) {

            int n;
            guint8 flags;
            /

            /
            do {
                proto_item *new_ie_flags_ti;
                proto_tree *new_ie_flags_tree;
                guint ies_found = 0;

                /
                new_ie_flags_ti = proto_tree_add_string_format(tree, hf_fp_hsdsch_new_ie_flags, tvb, offset, 1,
                                                              "", "New IE flags");
                new_ie_flags_tree = proto_item_add_subtree(new_ie_flags_ti, ett_fp_hsdsch_new_ie_flags);

                /
                flags = tvb_get_guint8(tvb, offset);
                /

                /
                for (n=0; n < 8; n++) {
                    proto_tree_add_item(new_ie_flags_tree, hf_fp_hsdsch_new_ie_flag[n], tvb, offset, 1, ENC_BIG_ENDIAN);
                    if ((flags >> (7-n)) & 0x01) {
                        ies_found++;
                    }
                }
                offset++;

                proto_item_append_text(new_ie_flags_ti, " (%u IEs found)", ies_found);

                /
            } while (0); /

            if (1) / {
                /
                proto_tree_add_item(tree, hf_fp_hsdsch_drt, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
            }
        }
        if (preferences_header_checksum) {
            verify_header_crc(tvb, pinfo, header_crc_pi, header_crc, header_length);
        }
        /
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}