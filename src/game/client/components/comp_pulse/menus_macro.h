#define BUTTON(section, button, height, id, text, value) \
    section.HSplitTop(height, &button, &section); \
    if(DoButton_CheckBox(id, text, value, &button)) \
        value ^= 1

#define BOX_LABEL(section, title_section, title, title_height, spacing_height) \
    section.HSplitTop(title_height, &title_section, &section); \
    Ui()->DoLabel(&title_section, Localize(title), 20.0f, TEXTALIGN_MC); \
    section.HSplitTop(spacing_height, nullptr, &section)

#define END_LABEL(section) \
    section.HSplitTop(20.0f, nullptr, &section)

#define DRAW_BOX(section, box_rect, height, color, margin) \
    section.HSplitTop(height, &box_rect, &section); \
    box_rect.Draw(color, IGraphics::CORNER_ALL, 10.0f); \
    box_rect.VMargin(margin, &box_rect)

#define SCROLLBAR(section, button, height, value, min, max, text) \
    section.HSplitTop(height, &button, &section); \
    Ui()->DoScrollbarOption(value, value, &button, Localize(text), min, max)

#define LASER_PREVIEW(section, preview_rect, height, inner_color, outline_color, laser_type) \
    section.HSplitTop(height, &preview_rect, &section); \
    section.HSplitTop(2 * MarginSmall, nullptr, &section); \
    DoLaserPreview(&preview_rect, inner_color, outline_color, laser_type)

#define FIND_KEY_BINDING(key_info, command_name) \
    key_info = CKeyInfo{command_name, command_name, 0, 0}; \
    for(int Mod = 0; Mod < CBinds::MODIFIER_COMBINATION_COUNT; Mod++) \
    { \
        for(int KeyId = 0; KeyId < KEY_LAST; KeyId++) \
        { \
            const char *pBind = m_pClient->m_Binds.Get(KeyId, Mod); \
            if(!pBind[0]) \
                continue; \
            if(str_comp(pBind, key_info.m_pCommand) == 0) \
            { \
                key_info.m_KeyId = KeyId; \
                key_info.m_ModifierCombination = Mod; \
                break; \
            } \
        } \
    }

