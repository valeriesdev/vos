[bits 32]                    ; Using 32-bit protected mode      | 32ビット保護モードの使用

VIDEO_MEMORY equ 0xb8000     ; The video memory is at 0xb8000   | ビデオメモリは0xb8000にあります
WHITE_OB_BLACK equ 0x0f      ; The default color of chars       | 文字のデフォルトの色

print_string_pm:             ; Print a string in protected mode | 保護モードで文字列を印刷する
    pusha                    ; Push all registers to the stack  | すべてのレジスタをスタックにプッシュします
    mov edx, VIDEO_MEMORY    ; Move the VRAM location to EDX    | VRAMの場所をEDXに移動します

print_string_pm_loop:        ; The main function loop           | 主な機能ループ
    mov al, [ebx]            ; Move &ebx into al                | ＆ebxをalに移動します
    mov ah, WHITE_OB_BLACK   ; Move the color byte into ah      | カラーバイトをahに移動します

    cmp al, 0                ; Check for string endbyte         | 文字列のエンドバイトを確認します
    je print_string_pm_done  ; ^                                | ^

    mov [edx], ax            ; Place the character in VRAM      | キャラクターをVRAMに配置します
    add ebx, 1               ; Next character                   | 次のキャラクター
    add edx, 2               ; Next video memory position       | 次のビデオメモリ位置

    jmp print_string_pm_loop ; Continue the loop                | ループを続ける

print_string_pm_done:        
    popa                     ; Pop from the stack               | スタックからポップ
    ret                      ; Return from function             | 機能から戻る