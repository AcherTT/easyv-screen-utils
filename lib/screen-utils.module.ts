import { DynamicModule, Module } from '@nestjs/common';
import { ScreenUtilsOptions } from './screen-utils.interface';
import { ScreenUtilsProviders } from './screen-utils.providers';
import { SCREEN_UTILS_PROVIDER } from './screen-utils.constants';

@Module({})
export class ScreenUtilsModule {
    static forRootAsync(options: ScreenUtilsOptions): DynamicModule {
        const provider = {
            provide: SCREEN_UTILS_PROVIDER,
            useValue: ScreenUtilsProviders.getInstance(options),
        }
        return {
            module: ScreenUtilsModule,
            providers: [provider],
            exports: [provider],
        };
    }
}