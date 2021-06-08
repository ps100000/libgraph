export type GraphID = number;

export interface Vec2 {
    x: number;
    y: number;
}

export interface GraphDef {
    valueCount: number;
    maxY: number;
    width: number;
    height: number;
    origin: Vec2;
}

export interface SpectrumMetadata {
    reference_key: string;
    timestamp: string;
    part_number: number;
    alarm_level: string;
    workstep: number;
}

export interface WebGLGraph {
    createGraph(def: GraphDef): GraphID;
    setGraphValues(id: GraphID, offset: number, values: number[]): boolean;
    removeGraph(id: GraphID): boolean;
    setWindowSize(width: number, height: number): void;

    addSpectrenValues(values: number[]): void;
    addReference(key: string, values: number[]): void;
    addSpectrumMetadata(meta: SpectrumMetadata): void;
}
