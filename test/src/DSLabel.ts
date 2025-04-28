export enum LabelFont {
    TSS16,
    TSS20,
    TSS24,
    TSS32,
    TST24
}
export enum ECCLevel {
    L, M, Q, H
}
export enum LabelLayout {
    Center,
    Left,
    Right,
    Top,
    Bottom,
}
export class LabelQRcode {
    type: string = "qrcode";
    x: number;
    y: number;
    eccLevel: ECCLevel;
    cellWidth: number;
    text: string;
    constructor(
        config: {
            x: number,
            y: number,
            text: string,
            eccLevel?: ECCLevel,
            cellWidth?: number,
        }) {
        this.x = config.x;
        this.y = config.y;
        this.text = config.text;
        this.eccLevel = config.eccLevel ?? ECCLevel.L;
        this.cellWidth = config.cellWidth ?? 1;
    }
}
export class LabelText {
    type: string = "text";
    x_start: number;
    y_start: number;
    x_end: number;
    y_end: number;
    font: LabelFont;
    rotation: number;
    xMultiplication: number;
    yMultiplication: number;
    texts: string[];
    hLayout: LabelLayout;
    vLayout: LabelLayout;
    constructor(
        config: {
            x_start: number,
            y_start: number,
            x_end: number,
            y_end: number,
            font: LabelFont,
            texts: string[]
            rotation?: number,
            xMultiplication?: number,
            yMultiplication?: number,
            hLayout?: LabelLayout,
            vLayout?: LabelLayout,
        }) {
        this.x_start = config.x_start;
        this.y_start = config.y_start;
        this.x_end = config.x_end;
        this.y_end = config.y_end;
        this.font = config.font;
        this.texts = config.texts;
        this.rotation = config.rotation ?? 0;
        this.xMultiplication = config.xMultiplication ?? 1;
        this.yMultiplication = config.yMultiplication ?? 1;
        this.hLayout = config.hLayout ?? LabelLayout.Left;
        this.vLayout = config.vLayout ?? LabelLayout.Top;
    }
}
export class LabelBox {
    type: string = "box";
    x_start: number;
    y_start: number;
    x_end: number;
    y_end: number;
    lineWidth: number;
    constructor(config: {
        x_start: number,
        y_start: number,
        x_end: number,
        y_end: number,
        lineWidth?: number,
    }) {
        this.x_start = config.x_start;
        this.y_start = config.y_start;
        this.x_end = config.x_end;
        this.y_end = config.y_end;
        this.lineWidth = config.lineWidth ?? 1;
    }
}
export class Label {
    width: number;
    height: number;
    gapM: number;
    gapN: number;
    num: number;
    title: string;
    constructor(title: string, width: number, height: number, gapM: number, gapN: number, num: number) {
        this.width = width;
        this.height = height;
        this.gapM = gapM;
        this.gapN = gapN;
        this.num = num;
        this.title = title;
    }
    elements: Object[] = [];
}